include(cmake/windows/generate_product_version.cmake)
generate_product_version(
        NAME "proxy-converter"
        BUNDLE "proxy-converter"
        COMPANY_NAME "proxy-converter"
        COMPANY_COPYRIGHT "proxy-converter"
        FILE_DESCRIPTION "CLI converter for sing-box proxy links"
)
add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)
if (MSVC)
    add_compile_options("/utf-8")
    add_definitions(-D_WIN32_WINNT=0x600 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
endif ()
