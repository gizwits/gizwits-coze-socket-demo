set(COMPONENT_SRCS "app_main.c" 
"sdk_callback/sdk_callback.c" "storage/user_storage.c"
)
set(COMPONENT_ADD_INCLUDEDIRS .)

idf_component_register(SRCS ${COMPONENT_SRCS}
                     INCLUDE_DIRS "." "../libs/includes"
                     REQUIRES  esp_http_server audio_recorder audio_board mqtt ota_service
                     input_key_service audio_pipeline audio_hal esp_wifi 
                     nvs_flash lwip esp_lcd bt json driver esp_peripherals 
                     audio_stream esp_system ${COMPONENT_REQUIRES})


set(SDK_DEPS esp_peripherals protocol_examples_common esp_http_server audio_recorder
    audio_board mqtt ota_service audio_pipeline 
    audio_hal esp_wifi nvs_flash lwip esp_lcd bt json driver 
    audio_stream esp_system ${COMPONENT_REQUIRES} main)

add_prebuilt_library(socket_sdk "../libs/libsocket_sdk.a" REQUIRES "${SDK_DEPS}")
add_prebuilt_library(audio_processor "../libs/libaudio_processor.a" REQUIRES "${SDK_DEPS}")

target_link_libraries(${COMPONENT_LIB} PRIVATE socket_sdk audio_processor)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wno-unused-variable -Wfatal-errors -Wno-incompatible-pointer-types -Wno-int-conversion -Wno-format -Wno-implicit-function-declaration -Wno-pointer-sign -Wno-double-promotion  -Wno-unused-value -Wno-switch -Wno-return-type -g -O3 -DUSE_SDK_LIB")  

spiffs_create_partition_image(flash_tone ../spiffs FLASH_IN_PROJECT)