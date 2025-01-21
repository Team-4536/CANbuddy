if(NOT CONFIG_PIX_LIB)
  return()
endif()

zephyr_library_named(pixlib)
zephyr_include_directories(
        ${ZEPHYR_PIXLIB_MODULE_DIR}/include
)

zephyr_library_sources(
        ${ZEPHYR_PIXLIB_MODULE_DIR}/src/generator.cpp
)
