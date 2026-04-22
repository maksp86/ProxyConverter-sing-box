#pragma once

#ifdef THRONE_CLI_HEADLESS

#include "include/global/Const.hpp"
#include "include/global/Utils.hpp"
#include <memory>
#include <srslist.h>
#include <string>

namespace Configs {
    struct CliSettingsRepo {
        bool mux_default_on = false;
        QString mux_protocol = "smux";
        int mux_concurrency = 8;
        bool mux_padding = false;

        QString utlsFingerprint;

        QString direct_dns_strategy;

        bool xray_mux_default_on = false;
        int xray_mux_concurrency = 8;

        Xray::XrayVlessPreference xray_vless_preference = Xray::XhttpOnly;
    };

    class DatabaseManager {
    public:
        std::unique_ptr<CliSettingsRepo> settingsRepo = std::make_unique<CliSettingsRepo>();
    };

    inline DatabaseManager *dataManager;

    void initDB(const std::string &dbPath);

    QString FindCoreRealPath();

    bool IsAdmin(bool forceRenew = false);

    bool isSetuidSet(const std::string &path);

    QString GetBasePath();

    bool HasNaive();
} // namespace Configs

#else

#include "third_party/Throne/include/global/Configs.hpp"

#endif

#define ROUTES_PREFIX_NAME QString("route_profiles")
#define ROUTES_PREFIX QString(ROUTES_PREFIX_NAME + "/")
