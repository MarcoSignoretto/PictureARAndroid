# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

SET(CMAKE_BUILD_TYPE Release)

# OpenCV stuff
#include_directories(native/jni/include)  # This line is required If want OpenCv library available also in C++ libraries (as PictureAR)
#add_library( lib_opencv SHARED IMPORTED )
#set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libopencv_java3.so)

#find_package(OpenCV 4.1 REQUIRED java)
message("Precessing Native CMake....")

find_package(OpenCV 4.1 REQUIRED java)
if (OpenCV_FOUND)
    message("here")
    message(STATUS "OpenCV Version ${OpenCV_VERSION} found.")
endif ()

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/native-lib.cpp)

add_library( # Sets the name of the library.
        PictureAR

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/utils.cpp
        src/main/cpp/boundary.cpp
        src/main/cpp/boundary_extractor.cpp
        src/main/cpp/marker.cpp
        src/main/cpp/Matcher.cpp)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(PictureAR

        ${OpenCV_LIBRARIES})


target_link_libraries( # Specifies the target library.
        native-lib

        ${OpenCV_LIBRARIES}
        PictureAR

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

message("Processed Native CMake")