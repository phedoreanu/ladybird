include(skia)
include(vulkan)

set(SOURCES
    AffineTransform.cpp
    Bitmap.cpp
    BitmapSequence.cpp
    CMYKBitmap.cpp
    Color.cpp
    ColorSpace.cpp
    Cursor.cpp
    Filter.cpp
    FontCascadeList.cpp
    Font/Font.cpp
    Font/FontData.cpp
    Font/FontDatabase.cpp
    Font/FontSupport.cpp
    Font/PathFontProvider.cpp
    Font/Typeface.cpp
    Font/TypefaceSkia.cpp
    Font/WOFF/Loader.cpp
    Font/WOFF2/Loader.cpp
    GradientPainting.cpp
    ImageFormats/AnimationWriter.cpp
    ImageFormats/BMPLoader.cpp
    ImageFormats/BMPWriter.cpp
    ImageFormats/BooleanDecoder.cpp
    ImageFormats/CCITTDecoder.cpp
    ImageFormats/GIFLoader.cpp
    ImageFormats/ICOLoader.cpp
    ImageFormats/ImageDecoder.cpp
    ImageFormats/JPEGLoader.cpp
    ImageFormats/JPEGXLLoader.cpp
    ImageFormats/JPEGWriter.cpp
    ImageFormats/PNGLoader.cpp
    ImageFormats/PNGWriter.cpp
    ImageFormats/TIFFLoader.cpp
    ImageFormats/TinyVGLoader.cpp
    ImageFormats/WebPLoader.cpp
    ImageFormats/WebPSharedLossless.cpp
    ImageFormats/WebPWriter.cpp
    ImageFormats/WebPWriterLossless.cpp
    ImageFormats/AVIFLoader.cpp
    ImmutableBitmap.cpp
    SkiaUtils.cpp
    PaintingSurface.cpp
    Palette.cpp
    Path.cpp
    PathSkia.cpp
    Painter.cpp
    PainterSkia.cpp
    Point.cpp
    Rect.cpp
    ShareableBitmap.cpp
    Size.cpp
    SystemTheme.cpp
    TextLayout.cpp
    Triangle.cpp
    VectorGraphic.cpp
    SkiaBackendContext.cpp
)

set(SWIFT_EXCLUDE_HEADERS
    FilterImpl.h
    GlobalFontConfig.h
    MetalContext.h
    VulkanContext.h
    SkiaUtils.h
)

if (APPLE)
    list(APPEND SOURCES MetalContext.mm)
endif()

if (HAS_VULKAN)
    list(APPEND SOURCES VulkanContext.cpp)
endif()

if (HAS_FONTCONFIG)
    list(APPEND SOURCES Font/GlobalFontConfig.cpp)
endif()

ladybird_lib(LibGfx gfx)

target_link_libraries(LibGfx PRIVATE LibCompress LibCore LibCrypto LibFileSystem LibTextCodec LibIPC LibUnicode)

set(generated_sources TIFFMetadata.h TIFFTagHandler.cpp)
list(TRANSFORM generated_sources PREPEND "ImageFormats/")

find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_custom_command(
        OUTPUT  ${generated_sources}
        COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/TIFFGenerator.py" -o "${CMAKE_CURRENT_BINARY_DIR}/ImageFormats"
        DEPENDS "TIFFGenerator.py"
        VERBATIM
)
target_sources(LibGfx PRIVATE ${generated_sources})
add_custom_target(generate_tiff_files_handler DEPENDS ${generated_sources})
add_dependencies(ladybird_codegen_accumulator generate_tiff_files_handler)
add_dependencies(LibGfx generate_tiff_files_handler)

set(generated_headers ${generated_sources})
list(FILTER generated_headers INCLUDE REGEX "\\.h$")
list(TRANSFORM generated_headers PREPEND "${CMAKE_CURRENT_BINARY_DIR}/")

if (ENABLE_INSTALL_HEADERS)
    list(TRANSFORM generated_sources PREPEND "${CMAKE_CURRENT_BINARY_DIR}/")
    install(FILES ${generated_sources} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/LibGfx/ImageFormats")
endif()

# Third-party
find_package(PkgConfig)

pkg_check_modules(WOFF2 REQUIRED IMPORTED_TARGET libwoff2dec)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(LIBAVIF REQUIRED)
find_package(WebP REQUIRED)
find_package(harfbuzz REQUIRED)

target_link_libraries(LibGfx PRIVATE PkgConfig::WOFF2 JPEG::JPEG PNG::PNG avif WebP::webp WebP::webpdecoder
            WebP::webpdemux WebP::libwebpmux skia harfbuzz)

if (HAS_FONTCONFIG)
    target_link_libraries(LibGfx PRIVATE Fontconfig::Fontconfig)
endif()

set(CMAKE_REQUIRED_LIBRARIES PNG::PNG)
check_c_source_compiles([=[
    #include <png.h>
    #if !defined(PNG_APNG_SUPPORTED) || !defined(PNG_READ_APNG_SUPPORTED) || !defined(PNG_READ_APNG_SUPPORTED)
    #error "APNG support is required"
    #endif
    int main() {}
]=] LIBPNG_HAS_APNG)
unset(CMAKE_REQUIRED_LIBRARIES)

if (NOT LIBPNG_HAS_APNG)
    message(FATAL_ERROR "libpng does not support APNG, which is required by LibGfx.")
endif()

if (NOT ANDROID)
    pkg_check_modules(Jxl REQUIRED IMPORTED_TARGET libjxl)
    if (WIN32)
        # FIXME: Build vcpkg libraries with clang-cl so they can also build in sanitize.
        # https://github.com/LadybirdBrowser/ladybird/issues/5224
        if (ENABLE_WINDOWS_CI)
            # We have to disable ASAN for the full target as fixing the issue in the initial SkiaBackendContext.cpp TU
            #  alone causes transitive issues in other TUs that depend on it
            target_compile_options(LibGfx PRIVATE -fno-sanitize=address)
        endif()
    endif()
    target_link_libraries(LibGfx PRIVATE PkgConfig::Jxl)
else()
    find_package(libjxl REQUIRED)
    find_package(hwy REQUIRED)
    target_link_libraries(LibGfx PRIVATE libjxl::libjxl hwy::hwy)
endif()

if (ENABLE_SWIFT)
    generate_clang_module_map(LibGfx GENERATED_FILES ${generated_headers} EXCLUDE_FILES ${SWIFT_EXCLUDE_HEADERS})
    target_sources(LibGfx PRIVATE
        Color.swift
    )
    target_link_libraries(LibGfx PRIVATE AK)
    add_swift_target_properties(LibGfx LAGOM_LIBRARIES AK)
endif()

if (APPLE)
    target_link_libraries(LibCore PUBLIC "-framework Metal")
endif()

if (HAS_VULKAN)
    target_link_libraries(LibCore PUBLIC Vulkan::Vulkan Vulkan::Headers)
endif()
