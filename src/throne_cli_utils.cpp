#include "include/global/Utils.hpp"

#include <QDateTime>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QMap>
#include <QRegularExpression>
#include <QTcpServer>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QUrlQuery>
#include <random>
#include <vector>

QStringList SplitLines(const QString &_string) {
    return _string.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);
}

QStringList SplitLinesSkipSharp(const QString &_string, int maxLine) {
    auto lines = SplitLines(_string);
    QStringList newLines;
    int i = 0;
    for (const auto &line : lines) {
        if (line.trimmed().startsWith("#")) {
            continue;
        }
        newLines << line;
        if (maxLine > 0 && ++i >= maxLine) {
            break;
        }
    }
    return newLines;
}

QByteArray DecodeB64IfValid(const QString &input, QByteArray::Base64Options options) {
    const auto result = QByteArray::fromBase64Encoding(
        input.toUtf8(), options | QByteArray::AbortOnBase64DecodingErrors);
    if (result) {
        return result.decoded;
    }
    return {};
}

QStringList SplitAndTrim(const QString &raw, const QString &separator, bool keepEmpty) {
    QStringList result;
    const auto parts = raw.split(separator);
    for (const auto &str : parts) {
        const auto trimmed = str.trimmed();
        if (!keepEmpty && trimmed.isEmpty()) {
            continue;
        }
        result << trimmed;
    }
    return result;
}

QString QStringList2Command(const QStringList &list) {
    QStringList escaped;
    for (auto str : list) {
        escaped << ("\"" + str.replace("\"", "\\\"") + "\"");
    }
    return escaped.join(" ");
}

QString GetQueryValue(const QUrlQuery &q, const QString &key, const QString &def) {
    const auto value = q.queryItemValue(key);
    return value.isEmpty() ? def : value;
}

QString GetRandomString(int randomStringLength) {
    std::random_device rd;
    std::mt19937 mt(rd());
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    std::uniform_int_distribution<int> dist(0, possibleCharacters.length() - 1);

    QString randomString;
    for (int i = 0; i < randomStringLength; ++i) {
        randomString.append(possibleCharacters.at(dist(mt)));
    }
    return randomString;
}

quint64 GetRandomUint64() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<quint64> dist;
    return dist(mt);
}

QJsonObject QString2QJsonObject(const QString &jsonString) {
    return QJsonDocument::fromJson(jsonString.toUtf8()).object();
}

QString QJsonObject2QString(const QJsonObject &jsonObject, bool compact) {
    return QJsonDocument(jsonObject).toJson(
        compact ? QJsonDocument::Compact : QJsonDocument::Indented);
}

QJsonArray QListStr2QJsonArray(const QList<QString> &list) {
    QVariantList values;
    bool hasValue = false;
    for (const auto &item : list) {
        if (item.trimmed().isEmpty()) {
            continue;
        }
        values.append(item);
        hasValue = true;
    }
    return hasValue ? QJsonArray::fromVariantList(values) : QJsonArray();
}

QJsonArray QListInt2QJsonArray(const QList<int> &list) {
    QVariantList values;
    for (const auto &item : list) {
        values.append(item);
    }
    return QJsonArray::fromVariantList(values);
}

QList<int> QJsonArray2QListInt(const QJsonArray &arr) {
    QList<int> list;
    for (const auto &item : arr) {
        list.append(item.toInt());
    }
    return list;
}

QList<QString> QJsonArray2QListString(const QJsonArray &arr) {
    QList<QString> list;
    for (const auto &item : arr) {
        list.append(item.toString());
    }
    return list;
}

QJsonArray QString2QJsonArray(const QString &str) {
    const auto doc = QJsonDocument::fromJson(str.toUtf8());
    return doc.isArray() ? doc.array() : QJsonArray();
}

QJsonObject QMapString2QJsonObject(const QMap<QString, QString> &mp) {
    QJsonObject result;
    for (auto it = mp.cbegin(); it != mp.cend(); ++it) {
        result.insert(it.key(), it.value());
    }
    return result;
}

QList<QString> QListInt2QListString(const QList<int> &list) {
    QList<QString> result;
    for (const auto &item : list) {
        result << Int2String(item);
    }
    return result;
}

QByteArray ReadFile(const QString &path) {
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        return {};
    }
    return file.readAll();
}

QString ReadFileText(const QString &path) {
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return {};
    }
    QTextStream stream(&file);
    return stream.readAll();
}

int MkPort() {
    QTcpServer server;
    server.listen();
    const auto port = server.serverPort();
    server.close();
    return port;
}

QList<int> MkManyPorts(int num) {
    QList<int> ports;
    std::vector<std::unique_ptr<QTcpServer>> servers;
    servers.reserve(num);
    for (int i = 0; i < num; ++i) {
        auto server = std::make_unique<QTcpServer>();
        if (!server->listen()) {
            continue;
        }
        ports.append(server->serverPort());
        servers.push_back(std::move(server));
    }
    return ports;
}

QString ReadableSize(const qint64 &size) {
    double sizeAsDouble = size;
    static const QStringList measures{
        "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"};
    QString measure = measures.first();
    for (auto it = measures.cbegin() + 1; sizeAsDouble >= 1024.0 && it != measures.cend(); ++it) {
        measure = *it;
        sizeAsDouble /= 1024.0;
    }
    return QString::fromLatin1("%1 %2").arg(sizeAsDouble, 0, 'f', 2).arg(measure);
}

bool IsIpAddress(const QString &str) {
    const auto address = QHostAddress(str);
    return address.protocol() == QAbstractSocket::IPv4Protocol
        || address.protocol() == QAbstractSocket::IPv6Protocol;
}

bool IsIpAddressV4(const QString &str) {
    return QHostAddress(str).protocol() == QAbstractSocket::IPv4Protocol;
}

bool IsIpAddressV6(const QString &str) {
    return QHostAddress(str).protocol() == QAbstractSocket::IPv6Protocol;
}

QString DisplayTime(long long time, int formatType) {
    QDateTime t;
    t.setMSecsSinceEpoch(time * 1000);
    return QLocale().toString(t, QLocale::FormatType(formatType));
}

QWidget *GetMessageBoxParent() {
    return nullptr;
}

int MessageBoxWarning(const QString &title, const QString &text) {
    qWarning().noquote() << title << text;
    return 0;
}

int MessageBoxInfo(const QString &title, const QString &text) {
    qInfo().noquote() << title << text;
    return 0;
}

void ActivateWindow(QWidget *w) {
    Q_UNUSED(w);
}

void HideWindow(QWidget *w) {
    Q_UNUSED(w);
}

void runOnUiThread(const std::function<void()> &callback, bool wait) {
    Q_UNUSED(wait);
    callback();
}

void runOnNewThread(const std::function<void()> &callback, bool wait) {
    if (!wait) {
        auto *thread = QThread::create([callback]() { callback(); });
        QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
        thread->start();
        return;
    }

    callback();
}

void runOnThread(const std::function<void()> &callback, QObject *parent, bool wait) {
    Q_UNUSED(parent);
    Q_UNUSED(wait);
    callback();
}

void setTimeout(const std::function<void()> &callback, QObject *obj, int timeout) {
    QTimer::singleShot(timeout, obj, callback);
}
