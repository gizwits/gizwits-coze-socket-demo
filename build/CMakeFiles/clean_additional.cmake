# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "esp-idf/esptool_py/flasher_args.json.in"
  "esp-idf/mbedtls/x509_crt_bundle"
  "flash_app_args"
  "flash_bootloader_args"
  "flash_tone.bin"
  "flasher_args.json"
  "ldgen_libraries"
  "ldgen_libraries.in"
  "littlefs_py_venv"
  "main_app.map"
  "project_elf_src_esp32s3.c"
  "wifi_configuration.html.S"
  "wifi_configuration_done.html.S"
  "x509_crt_bundle.S"
  )
endif()
