cmake_minimum_required (VERSION 3.28.0)

project(Ros2Native)

include (FetchContent)

set (CARLA_DEPENDENCIES_PENDING)

macro (carla_dependency_add NAME TAG ARCHIVE_URL GIT_URL)
  if (PREFER_CLONE)
    message ("Cloning ${NAME}...")
    FetchContent_Declare(
      ${NAME}
      GIT_REPOSITORY ${GIT_URL}
      GIT_TAG ${TAG}
      GIT_SUBMODULES_RECURSE ON
      GIT_SHALLOW ON
      GIT_PROGRESS ON
      OVERRIDE_FIND_PACKAGE
      ${ARGN}
    )
    list (APPEND CARLA_DEPENDENCIES_PENDING ${NAME})
  else ()
    message ("Downloading ${NAME}...")
    FetchContent_Declare(
      ${NAME}
      URL ${ARCHIVE_URL}
      OVERRIDE_FIND_PACKAGE
      ${ARGN}
    )
    list (APPEND CARLA_DEPENDENCIES_PENDING ${NAME})
  endif ()
endmacro ()

macro (carla_dependencies_make_available)
  FetchContent_MakeAvailable (
    ${CARLA_DEPENDENCIES_PENDING})
  set (CARLA_DEPENDENCIES_PENDING)
endmacro ()

macro (carla_fetchcontent_option NAME VALUE)
  set (${NAME} ${VALUE} CACHE INTERNAL "")
endmacro ()



carla_fetchcontent_option (CMAKE_CXX_FLAGS_RELEASE -D_GLIBCXX_USE_CXX11_ABI=0)
carla_fetchcontent_option (BUILD_SHARED_LIBS ON)
carla_fetchcontent_option (FOONATHAN_MEMORY_FORCE_VENDORED_BUILD ON)
carla_dependency_add (
  foonathan_memory
  https://github.com/foonathan/memory.git
  https://github.com/foonathan/memory/archive/refs/tags/v0.7-3.zip
  v0.7-3
)

carla_dependency_add (
  fastcdr
  https://github.com/eProsima/Fast-CDR.git
  https://github.com/eProsima/Fast-CDR/archive/refs/heads/1.1.x.zip
  1.1.x
)

carla_dependencies_make_available ()
FetchContent_MakeAvailable ("foo_mem-ext")

carla_fetchcontent_option (CMAKE_CXX_FLAGS -latomic THIRDPARTY_Asio=FORCE THIRDPARTY_TinyXML2=FORCE)
#carla_fetchcontent_option (CMAKE_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/_deps/fast-cdr-build)
include_directories (${foonathan_memory_SOURCE_DIR} ${foonathan_memory_BINARY_DIR}) # @TODO HACK
include_directories (${fastcdr_BINARY_DIR}"/foo_mem-ext-prefix/src/foo_mem-ext/include") # @TODO HACK

carla_dependency_add (
  fast-dds
  https://github.com/eProsima/Fast-DDS.git
  https://github.com/eProsima/Fast-DDS/archive/refs/heads/2.11.2.zip
  2.11.2
  #BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/fast-cdr-build/src/cpp/
)

carla_dependencies_make_available ()
