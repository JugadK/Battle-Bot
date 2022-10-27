
idf_component_register(
        SRCS "main.c" "hid_host_demo.c" "ps4/ps4_parser.c" "enums/controllers.h" "tb6612fng/tb6612fng.h" "tb6612fng/tb6612fng.c" "drive_control/drive_control.h" "drive_control/drive_control.c" 
        INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}")
