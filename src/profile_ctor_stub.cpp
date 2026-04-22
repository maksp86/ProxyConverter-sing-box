#include "include/database/entities/Profile.h"

namespace Configs {
Profile::Profile(Configs::outbound *outbound, const QString &type_) {
    if (!type_.isEmpty()) {
        type = type_;
    }

    if (outbound != nullptr) {
        this->outbound = std::shared_ptr<Configs::outbound>(outbound);
    }
}
} // namespace Configs
