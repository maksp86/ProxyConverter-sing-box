#include <cstdio>
#include <cstdlib>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

#include "include/configs/common/utils.h"
#include "include/global/Configs.hpp"
#include "include/database/entities/Profile.h"

namespace {
struct CliOptions {
    QString inputLines;
    QString inputJson;
    QString output = "-";
    int startPort = 10808;
    bool changePorts = false;
    QString target = "sing-box"; // sing-box|xray
    QStringList urls;
};

QString readAll(const QString& path) {
    if (path == "-") {
        QTextStream in(stdin);
        return in.readAll();
    }
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
    return QString::fromUtf8(file.readAll());
}

bool writeAll(const QString& path, const QByteArray& data) {
    if (path == "-") {
        QTextStream out(stdout);
        out << data;
        return true;
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    file.write(data);
    return true;
}

std::shared_ptr<Configs::Profile> newProfileByType(const QString& type) {
    using namespace Configs;
    outbound* out = nullptr;
    if (type == "socks") out = new socks();
    else if (type == "http") out = new http();
    else if (type == "shadowsocks") out = new shadowsocks();
    else if (type == "vmess") out = new vmess();
    else if (type == "vless") out = new vless();
    else if (type == "xrayvless") out = new xrayVless();
    else if (type == "trojan") out = new Trojan();
    else if (type == "anytls") out = new anyTLS();
    else if (type == "hysteria") out = new hysteria();
    else if (type == "tuic") out = new tuic();
    else if (type == "juicity") out = new juicity();
    else if (type == "trusttunnel") out = new trusttunnel();
    else if (type == "shadowtls") out = new shadowtls();
    else if (type == "wireguard") out = new wireguard();
    else if (type == "ssh") out = new ssh();
    else if (type == "naive") out = new naive();
    else out = new outbound();

    return std::make_shared<Profile>(out, type);
}

bool preferXrayVless(const QString& link) {
    auto url = QUrl(link);
    if (!url.isValid()) return false;
    auto q = QUrlQuery(url.query());
    return q.queryItemValue("type") == "xhttp" || q.queryItemValue("security") == "reality";
}

std::shared_ptr<Configs::Profile> parseSingle(const QString& raw) {
    const auto str = raw.trimmed();
    if (str.isEmpty() || str.startsWith("#") || str.startsWith("//")) return nullptr;

    std::shared_ptr<Configs::Profile> p;
    if (str.startsWith("socks5://") || str.startsWith("socks4://") || str.startsWith("socks4a://") || str.startsWith("socks://")) {
        p = newProfileByType("socks");
        if (!p->Socks()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("http://") || str.startsWith("https://")) {
        p = newProfileByType("http");
        if (!p->Http()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("ss://")) {
        p = newProfileByType("shadowsocks");
        if (!p->ShadowSocks()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("vmess://")) {
        p = newProfileByType("vmess");
        if (!p->VMess()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("vless://")) {
        p = newProfileByType(preferXrayVless(str) ? "xrayvless" : "vless");
        bool ok = p->type == "xrayvless" ? p->XrayVLESS()->ParseFromLink(str) : p->VLESS()->ParseFromLink(str);
        if (!ok) return nullptr;
    } else if (str.startsWith("trojan://")) {
        p = newProfileByType("trojan");
        if (!p->Trojan()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("anytls://")) {
        p = newProfileByType("anytls");
        if (!p->AnyTLS()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("hysteria://") || str.startsWith("hysteria2://") || str.startsWith("hy2://")) {
        p = newProfileByType("hysteria");
        if (!p->Hysteria()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("tuic://")) {
        p = newProfileByType("tuic");
        if (!p->TUIC()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("juicity://")) {
        p = newProfileByType("juicity");
        if (!p->Juicity()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("tt://")) {
        p = newProfileByType("trusttunnel");
        if (!p->TrustTunnel()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("shadowtls://")) {
        p = newProfileByType("shadowtls");
        if (!p->ShadowTLS()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("wg://")) {
        p = newProfileByType("wireguard");
        if (!p->Wireguard()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("ssh://")) {
        p = newProfileByType("ssh");
        if (!p->SSH()->ParseFromLink(str)) return nullptr;
    } else if (str.startsWith("naive+https://") || str.startsWith("naive+quic://")) {
        p = newProfileByType("naive");
        if (!p->Naive()->ParseFromLink(str)) return nullptr;
    } else {
        return nullptr;
    }

    p->outbound->name = p->outbound->name.isEmpty() ? p->outbound->DisplayAddress() : p->outbound->name;
    return p;
}

CliOptions parseArgs(QCoreApplication& app) {
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption({"input-lines", "Read links line-by-line from a file or stdin (-).", "path"});
    parser.addOption({"input-json", "Read links as a JSON string array from a file or stdin (-).", "path"});
    parser.addOption({{"o", "output"}, "Output JSON to a file or stdout (-).", "path", "-"});
    parser.addOption({"start-port", "Starting port for Xray inbounds.", "port", "10808"});
    parser.addOption({"change-ports", "Enable sequential port changing (+1 for each successful config)."});
    parser.addOption({"target", "Generation target: sing-box or xray.", "target", "sing-box"});
    parser.addPositionalArgument("urls", "Positional arguments for proxy URLs.");
    if (!parser.parse(app.arguments())) {
        QTextStream(stderr) << parser.errorText() << "\n\n" << parser.helpText();
        std::fflush(stdout);
        std::fflush(stderr);
        std::_Exit(1);
    }
    if (parser.isSet("help")) {
        QTextStream(stdout) << parser.helpText();
        std::fflush(stdout);
        std::fflush(stderr);
        std::_Exit(0);
    }

    CliOptions o;
    o.inputLines = parser.value("input-lines");
    o.inputJson = parser.value("input-json");
    o.output = parser.value("output");
    o.startPort = parser.value("start-port").toInt();
    o.changePorts = parser.isSet("change-ports");
    o.target = parser.value("target").trimmed().toLower();
    o.urls = parser.positionalArguments();
    if (o.target != "sing-box" && o.target != "xray") {
        o.target = "sing-box";
    }
    return o;
}

QStringList collectInputs(const CliOptions& o) {
    if (!o.inputLines.isEmpty()) {
        return readAll(o.inputLines).split('\n');
    }
    if (!o.inputJson.isEmpty()) {
        const auto doc = QJsonDocument::fromJson(readAll(o.inputJson).toUtf8());
        QStringList list;
        if (doc.isArray()) {
            for (const auto& v : doc.array()) if (v.isString()) list << v.toString();
        }
        return list;
    }
    return o.urls;
}
} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    Configs::DatabaseManager cliSettings;
    Configs::dataManager = &cliSettings;
    MW_show_log = [](QString) {};
    MW_dialog_message = [](QString, QString) {};

    const CliOptions opts = parseArgs(app);
    const auto inputs = collectInputs(opts);

    QJsonArray result;
    int currentPort = opts.startPort;

    for (const auto& input : inputs) {
        QJsonObject item;
        item["input"] = input;

        auto profile = parseSingle(input);
        if (!profile) {
            item["success"] = false;
            item["error"] = "Unsupported or invalid link.";
            result.append(item);
            continue;
        }

        item["success"] = true;
        item["type"] = profile->type;
        item["name"] = profile->outbound->DisplayName();

        if (opts.target == "sing-box") {
            const auto built = profile->outbound->Build();
            item["sing_box"] = built.object;
            if (!built.error.isEmpty()) item["sing_box_error"] = built.error;
        }

        if (opts.target == "xray") {
            const auto builtX = profile->outbound->BuildXray();
            if (builtX.object.isEmpty()) {
                item["xray_error"] = builtX.error.isEmpty() ? "Xray generation is not available for this protocol." : builtX.error;
            } else {
                item["xray_outbound"] = builtX.object;
                QJsonObject inbound{{"type", "socks"}, {"tag", "in"}, {"listen", "127.0.0.1"}, {"port", currentPort}};
                item["xray_inbound"] = inbound;
                if (opts.changePorts) currentPort++;
            }
        }

        result.append(item);
    }

    const QJsonDocument outDoc(result);
    if (!writeAll(opts.output, outDoc.toJson(QJsonDocument::Indented))) {
        QTextStream(stderr) << "Failed to write output\n";
        std::fflush(stdout);
        std::fflush(stderr);
        std::_Exit(1);
    }
    std::fflush(stdout);
    std::fflush(stderr);
    std::_Exit(0);
}
