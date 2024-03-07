project(
  carla-ros2-native
  LANGUAGES CXX
  VERSION ${CARLA_VERSION}
)

macro (carla_dependency_add NAME URL TAG)
  message ("Fetching ${NAME}...")
  FetchContent_Declare(
    ${NAME}
    GIT_SUBMODULES_RECURSE ON
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    GIT_REPOSITORY ${URL}
    GIT_TAG ${TAG}
    OVERRIDE_FIND_PACKAGE
  )
  list (APPEND CARLA_DEPENDENCIES ${NAME})
endmacro ()

macro (carla_dependencies_make_available)
  FetchContent_MakeAvailable (
    ${CARLA_DEPENDENCIES})
  set (CARLA_DEPENDENCIES)
endmacro ()

carla_dependency_add(
  foonathan-memory-vendor
  https://github.com/eProsima/foonathan_memory_vendor.git
  master
)
carla_dependencies_make_available()
