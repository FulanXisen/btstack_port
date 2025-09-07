cmake_minimum_required(VERSION 3.12)
project(BTstack-port)

# --------------------------
# 基础配置
# --------------------------
# 生成编译命令文件，方便IDE索引（如VS Code）
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 路径配置（核心！请根据实际环境修改）
# BTSTACK_ROOT: BTStack源码根目录
# DEMO_ROOT: 当前示例程序所在目录（无需修改）
SET(BTSTACK_ROOT "/Users/fanyx/toys/btstack_v1.6.2")
SET(DEMO_ROOT ${CMAKE_CURRENT_SOURCE_DIR})


# --------------------------
# 编译器配置
# --------------------------
# 根据不同编译器设置警告级别（与BTStack官方保持一致）
message("CMAKE_C_COMPILER_ID: ${CMAKE_C_COMPILER_ID}")
if("${CMAKE_C_COMPILER_ID}" MATCHES ".*Clang.*")
    # Clang编译器警告配置
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-variable -Wswitch-default -Wall -Wextra -Werror -Werror=uninitialized")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    # GCC编译器警告配置（树莓派默认使用GCC）
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-but-set-variable -Wunused-variable -Wswitch-default -Wall -Wextra -Werror")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
    # Intel编译器（预留）
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    # Visual Studio编译器（预留）
endif()


# --------------------------
# 头文件路径配置
# --------------------------
# 包含所有必要的头文件目录，确保编译器能找到所有.h文件
include_directories(
    # 当前示例程序目录（包含btstack_config.h和main.c等）
    ${DEMO_ROOT}

    # BTStack第三方依赖库头文件
    ${BTSTACK_ROOT}/3rd-party/micro-ecc           # 椭圆曲线加密库
    ${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/include  # Bluedroid解码库
    ${BTSTACK_ROOT}/3rd-party/bluedroid/encoder/include  # Bluedroid编码库
    ${BTSTACK_ROOT}/3rd-party/lc3-google/include  # LC3音频编码库
    ${BTSTACK_ROOT}/3rd-party/md5                 # MD5哈希库
    ${BTSTACK_ROOT}/3rd-party/hxcmod-player       # 音频播放器库
    ${BTSTACK_ROOT}/3rd-party/hxcmod-player/mod   # 音频模块
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/include  # LwIP网络协议栈
    ${BTSTACK_ROOT}/3rd-party/lwip/dhcp-server    # DHCP服务器模块
    ${BTSTACK_ROOT}/3rd-party/qr-code-generator   # 二维码生成库
    ${BTSTACK_ROOT}/3rd-party/rijndael            # AES加密库
    ${BTSTACK_ROOT}/3rd-party/yxml                # XML解析库
    ${BTSTACK_ROOT}/3rd-party/tinydir             # 目录遍历库

    # BTStack核心源码目录
    ${BTSTACK_ROOT}/src                           # 核心功能模块
    ${BTSTACK_ROOT}/chipset/realtek               # Realtek芯片支持
    ${BTSTACK_ROOT}/chipset/zephyr                # Zephyr芯片支持

    # BTStack平台相关目录
    ${BTSTACK_ROOT}/platform/posix                # POSIX系统适配
    ${BTSTACK_ROOT}/platform/embedded             # 嵌入式平台适配
    ${BTSTACK_ROOT}/platform/lwip                 # LwIP网络适配
    ${BTSTACK_ROOT}/platform/lwip/port            # LwIP端口适配

    # BTStack libusb端口相关目录（USB蓝牙核心）
    ${BTSTACK_ROOT}/port/libusb
    ${BTSTACK_ROOT}/platform/libusb
)


# --------------------------
# 源码收集与处理
# --------------------------
# 收集BTStack核心源码文件（按模块分类，结构清晰）

# 1. 核心功能模块
file(GLOB SOURCES_SRC 
    "${BTSTACK_ROOT}/src/*.c" 
    "${BTSTACK_ROOT}/example/sco_demo_util.c"  # SCO音频演示工具
)

# 2. BLE相关模块
file(GLOB SOURCES_BLE 
    "${BTSTACK_ROOT}/src/ble/*.c" 
)

# 3. GATT服务模块
file(GLOB SOURCES_GATT 
    "${BTSTACK_ROOT}/src/ble/gatt-service/*.c" 
)

# 4. 传统蓝牙模块
file(GLOB SOURCES_CLASSIC 
    "${BTSTACK_ROOT}/src/classic/*.c" 
)

# 5. Mesh网络模块
file(GLOB SOURCES_MESH 
    "${BTSTACK_ROOT}/src/mesh/*.c" 
    "${BTSTACK_ROOT}/src/mesh/gatt-service/*.c" 
)

# 6. 第三方库源码
file(GLOB SOURCES_BLUEDROID  # Bluedroid编解码器
    "${BTSTACK_ROOT}/3rd-party/bluedroid/encoder/srce/*.c" 
    "${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/srce/*.c" 
)
file(GLOB SOURCES_MD5        # MD5库
    "${BTSTACK_ROOT}/3rd-party/md5/md5.c" 
)
file(GLOB SOURCES_UECC       # 椭圆加密库
    "${BTSTACK_ROOT}/3rd-party/micro-ecc/uECC.c" 
)
file(GLOB SOURCES_YXML       # XML解析库
    "${BTSTACK_ROOT}/3rd-party/yxml/yxml.c" 
)
file(GLOB SOURCES_HXCMOD     # 音频播放库
    "${BTSTACK_ROOT}/3rd-party/hxcmod-player/*.c"  
    "${BTSTACK_ROOT}/3rd-party/hxcmod-player/mods/*.c" 
)
file(GLOB SOURCES_RIJNDAEL   # AES加密库
    "${BTSTACK_ROOT}/3rd-party/rijndael/rijndael.c" 
)
file(GLOB SOURCES_LC3_GOOGLE # LC3音频编码
    "${BTSTACK_ROOT}/3rd-party/lc3-google/src/*.c" 
)

# 7. 平台与硬件相关源码
file(GLOB SOURCES_POSIX      # POSIX平台适配
    "${BTSTACK_ROOT}/platform/posix/*.c" 
)
file(GLOB SOURCES_LIBUSB     # LibUSB端口实现（核心）
    "${BTSTACK_ROOT}/port/libusb/*.c" 
    "${BTSTACK_ROOT}/platform/libusb/*.c" 
)
file(GLOB SOURCES_ZEPHYR     # Zephyr芯片驱动
    "${BTSTACK_ROOT}/chipset/zephyr/*.c" 
)
file(GLOB SOURCES_REALTEK    # Realtek芯片驱动
    "${BTSTACK_ROOT}/chipset/realtek/*.c" 
)

# 8. LwIP网络协议栈源码（按需启用）
set(LWIP_CORE_SRC 
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/def.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/inet_chksum.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/init.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ip.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/mem.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/memp.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/netif.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/pbuf.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/tcp.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/tcp_in.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/tcp_out.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/timeouts.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/udp.c
)
set(LWIP_IPV4_SRC
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/acd.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/dhcp.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/etharp.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/icmp.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/ip4.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/ip4_addr.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/ip4_frag.c
)
set(LWIP_NETIF_SRC
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/netif/ethernet.c
)
set(LWIP_HTTPD
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/apps/http/altcp_proxyconnect.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/apps/http/fs.c
    ${BTSTACK_ROOT}/3rd-party/lwip/core/src/apps/http/httpd.c
)
set(LWIP_DHCPD
    ${BTSTACK_ROOT}/3rd-party/lwip/dhcp-server/dhserver.c
)
set(LWIP_PORT
    ${BTSTACK_ROOT}/platform/lwip/port/sys_arch.c
    ${BTSTACK_ROOT}/platform/lwip/bnep_lwip.c
)
set(SOURCES_LWIP 
    ${LWIP_CORE_SRC} 
    ${LWIP_IPV4_SRC} 
    ${LWIP_NETIF_SRC} 
    ${LWIP_HTTPD} 
    ${LWIP_DHCPD} 
    ${LWIP_PORT}
)


# --------------------------
# 源码过滤（移除不需要的文件）
# --------------------------
# 移除其他HCI传输实现（只保留libusb传输）
file(GLOB SOURCES_SRC_OFF "${BTSTACK_ROOT}/src/hci_transport_*.c")
list(REMOVE_ITEM SOURCES_SRC ${SOURCES_SRC_OFF})

# 移除BLE设备数据库的内存实现（避免冲突）
file(GLOB SOURCES_BLE_OFF "${BTSTACK_ROOT}/src/ble/le_device_db_memory.c")
list(REMOVE_ITEM SOURCES_BLE ${SOURCES_BLE_OFF})

# 移除POSIX平台的文件系统设备数据库（避免冲突）
file(GLOB SOURCES_POSIX_OFF "${BTSTACK_ROOT}/platform/posix/le_device_db_fs.c")
list(REMOVE_ITEM SOURCES_POSIX ${SOURCES_POSIX_OFF})


# --------------------------
# 构建BTStack静态库
# --------------------------
# 合并所有源码并排序（确保编译顺序一致性）
set(SOURCES_BTSTACK 
    ${SOURCES_MD5}
    ${SOURCES_YXML}
    ${SOURCES_BLUEDROID}
    ${SOURCES_POSIX}
    ${SOURCES_RIJNDAEL}
    ${SOURCES_LIBUSB}
    ${SOURCES_LC3_GOOGLE}
    ${SOURCES_SRC}
    ${SOURCES_BLE}
    ${SOURCES_GATT}
    ${SOURCES_MESH}
    ${SOURCES_CLASSIC}
    ${SOURCES_UECC}
    ${SOURCES_HXCMOD}
    ${SOURCES_REALTEK}
    ${SOURCES_ZEPHYR}
)
list(SORT SOURCES_BTSTACK)
# 生成BTStack静态库（先编译库再链接，与官方保持一致）
add_library(btstack STATIC ${SOURCES_BTSTACK})


# --------------------------
# 依赖库链接
# --------------------------
# 使用pkg-config管理系统库依赖
find_package(PkgConfig REQUIRED)

# 1. LibUSB-1.0（USB蓝牙适配器必需）
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
target_include_directories(btstack PUBLIC ${LIBUSB_INCLUDE_DIRS})
target_link_directories(btstack PUBLIC ${LIBUSB_LIBRARY_DIRS})
target_link_libraries(btstack ${LIBUSB_LIBRARIES})

# 2. PortAudio（音频处理，A2DP必需）
pkg_check_modules(PORTAUDIO portaudio-2.0)
if(PORTAUDIO_FOUND)
    message("HAVE_PORTAUDIO: 已找到PortAudio库，启用音频支持")
    target_include_directories(btstack PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
    target_link_directories(btstack PUBLIC ${PORTAUDIO_LIBRARY_DIRS})
    target_link_libraries(btstack ${PORTAUDIO_LIBRARIES})
    add_compile_definitions(HAVE_PORTAUDIO)  # 启用PortAudio宏定义
else()
    message(WARNING "未找到PortAudio库，音频功能将不可用")
endif()

# 3. 线程库（POSIX平台多线程支持）
find_package(Threads)
target_link_libraries(btstack ${CMAKE_THREAD_LIBS_INIT})

# 4. 数学库（BTStack内部计算依赖）
target_link_libraries(btstack m)

    
add_subdirectory(a2dp)
add_subdirectory(avrcp)
add_subdirectory(hci)

# --------------------------
# 构建A2DP Sink示例程序
# --------------------------
# 示例程序源码（根据实际文件调整）
set(DEMO_SOURCES
    ${DEMO_ROOT}/btstack_global.c
    ${DEMO_ROOT}/btstack_main.c  
    ${DEMO_ROOT}/main.c          
)

# 生成可执行文件
add_executable(btstack_port ${DEMO_SOURCES})

# 链接BTStack静态库和所有依赖
target_link_libraries(btstack_port 
    btstack
    a2dp
    avrcp
    hci
    )

# 示例程序编译宏定义（核心功能开关）
target_compile_definitions(btstack_port PRIVATE
    HAVE_BTSTACK_CONFIG_H  # 使用当前目录的btstack_config.h配置文件
    PLATFORM_LINUX         # 声明运行在Linux平台
    ENABLE_SBC_DECODER     # 启用SBC音频解码（A2DP必需）
)

