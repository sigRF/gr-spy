INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_SPY spy)

FIND_PATH(
    SPY_INCLUDE_DIRS
    NAMES spy/api.h
    HINTS $ENV{SPY_DIR}/include
        ${PC_SPY_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SPY_LIBRARIES
    NAMES gnuradio-spy
    HINTS $ENV{SPY_DIR}/lib
        ${PC_SPY_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SPY DEFAULT_MSG SPY_LIBRARIES SPY_INCLUDE_DIRS)
MARK_AS_ADVANCED(SPY_LIBRARIES SPY_INCLUDE_DIRS)

