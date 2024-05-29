project (
  carla-unreal
  VERSION
    ${CARLA_VERSION}
  LANGUAGES
    CXX
  DESCRIPTION
    "Open-source simulator for autonomous driving research."
)

carla_option (
  ENABLE_DIRECTORY_CLEAN
  "Enable ADDITIONAL_CLEAN_FILES on directory level"
  ON
)

set (
  CARLA_BUILD_PATH
  ${CARLA_WORKSPACE_PATH}/Build/
)

set (
  CARLA_UE_PATH
  ${CARLA_WORKSPACE_PATH}/Unreal/CarlaUnreal
)

set (
  CARLA_UE_PLUGINS_PATH
  ${CARLA_UE_PATH}/Plugins
)

set (
  CARLA_UE_CARLA_PATH
  ${CARLA_UE_PLUGINS_PATH}/Carla
)

set (
  CARLA_UE_CARLA_TOOLS_PATH
  ${CARLA_UE_PLUGINS_PATH}/CarlaTools
)

set (
  CARLA_UE_CARLA_EXPORTER_PATH
  ${CARLA_UE_PLUGINS_PATH}/CarlaExporter
)

set (
  CARLA_UE_PROJECT_PATH
  ${CARLA_UE_PATH}/CarlaUnreal.uproject
)

set (
  CARLA_UE_CONTENT_PATH
  ${CARLA_UE_PATH}/Content/Carla
)

if (NOT EXISTS ${CARLA_UE_CONTENT_PATH}/.git)
  carla_error ("Missing CARLA content.")
endif ()

if (ENABLE_ROS2)
  set (LAUNCH_ARGS ${LAUNCH_ARGS} --ros2)
endif()

if (ENABLE_ROS2_DEMO)
  set (LAUNCH_ARGS ${LAUNCH_ARGS} --ros2-demo)
endif()

if (WIN32)
  set (EXE_EXT .exe)
  set (UE_SYSTEM_NAME Win64)
elseif (LINUX)
  set (EXE_EXT)
  set (UE_SYSTEM_NAME Linux)
elseif (APPLE)
  set (EXE_EXT)
  set (UE_SYSTEM_NAME Mac)
else ()
  carla_error ("Unknown target system.")
endif ()

set (
  UNREAL_EDITOR_PATH
  ${CARLA_UNREAL_ENGINE_PATH}/Engine/Binaries/${UE_SYSTEM_NAME}/UnrealEditor${EXE_EXT}
)

set (CARLA_UE_CONTENT_CHECK_PATH ${CARLA_UE_PATH}/Content/Carla/Blueprints)
if (IS_DIRECTORY ${CARLA_UE_CONTENT_CHECK_PATH})
  carla_message ("CARLA Unreal Content - FOUND")
else ()
  carla_message ("CARLA Unreal Content - MISSING")
  carla_warning ("Could not find CARLA Content, download it or running the editor will result in a crash.")
endif ()



set (
  UE_DEPENDENCIES_ORDER_ONLY
  carla-server
  libsqlite3
  Boost::asio
  Boost::python
  Boost::geometry
  Boost::gil
  Eigen3::Eigen
  zlibstatic
  png_static
  rpc
)

set (UE_DEPENDENCIES ${UE_DEPENDENCIES_ORDER_ONLY})

if (BUILD_CARLA_CLIENT)
  list (APPEND UE_DEPENDENCIES_ORDER_ONLY carla-client)
endif ()



define_property (
  TARGET
  PROPERTY
    UE_SOURCE_DIR
  BRIEF_DOCS
    "Target property that holds the root source path of a CARLA UE subproject."
)



set (CARLA_DEFINED_UE_PROPERTIES)

macro (carla_define_ue_property
  VARIABLE_NAME_UE)

  list (
    APPEND
    CARLA_DEFINED_UE_PROPERTIES
    ${VARIABLE_NAME_UE}
  )

  define_property (
    TARGET
    PROPERTY
      UE_PROPERTY_${VARIABLE_NAME_UE}
    BRIEF_DOCS
      "Target property meant to mirror UBT's ${UE_VARIABLE_NAME}"
  )

  function (carla_get_ue_${VARIABLE_NAME_UE} OUT_VAR TARGET)
    get_target_property (
      TEMP
      ${TARGET}
      UE_PROPERTY_${VARIABLE_NAME_UE}
    )
    set (${OUT_VAR} ${TEMP})
    return (PROPAGATE ${OUT_VAR})
  endfunction ()
  
  function (carla_save_ue_${VARIABLE_NAME_UE} TARGET)
    get_target_property (
      TEMP
      ${TARGET}
      UE_PROPERTY_${VARIABLE_NAME_UE}
    )
    if (TEMP)
      get_target_property (
        UE_SOURCE_DIR_VALUE
        ${TARGET}
        UE_SOURCE_DIR
      )
      if (NOT IS_DIRECTORY "${UE_SOURCE_DIR_VALUE}")
        carla_error ("Missing UE_SOURCE_DIR target property in ${TARGET}.")
      endif ()
      file (
        GENERATE
        OUTPUT
          ${UE_SOURCE_DIR_VALUE}/${TARGET}-${VARIABLE_NAME_UE}.txt
        CONTENT
          "${TEMP}"
      )
    endif ()
  endfunction ()
  
  function (carla_add_ue_${VARIABLE_NAME_UE} TARGET)
    get_target_property (
      TEMP
      ${TARGET}
      UE_PROPERTY_${VARIABLE_NAME_UE}
    )
    if (TEMP)
      list (
        APPEND
        TEMP
        ${ARGN}
      )
    endif ()
    set_target_properties (
      ${TARGET}
      PROPERTIES
        UE_PROPERTY_${VARIABLE_NAME_UE} ${TEMP}
    )
  endfunction ()

endmacro ()

macro (carla_generate_ue_property_files TARGET)
  foreach (UE_PROPERTY ${CARLA_DEFINED_UE_PROPERTIES})
    cmake_language (
      CALL
      carla_save_ue_${UE_PROPERTY}
      ${TARGET}
    )
  endforeach ()
endmacro ()



if (WIN32)
  set (
    CARLA_UE_GENERATE_PROJECT_FILES_COMMAND
    ${CARLA_UNREAL_ENGINE_PATH}/GenerateProjectFiles.bat
      -project=${CARLA_UE_PROJECT_PATH}
      -CMakefile
      -game
      -engine
  )
  set (
    CARLA_UE_BUILD_COMMAND_PREFIX
      ${CARLA_UNREAL_ENGINE_PATH}/Engine/Build/BatchFiles/Build.bat
  )
  set (
    CARLA_UE_UAT_COMMAND_PREFIX
      ${CARLA_UNREAL_ENGINE_PATH}/Engine/Build/BatchFiles/RunUAT.bat
  )
else ()
  set (
    CARLA_UE_GENERATE_PROJECT_FILES_COMMAND
    ${CARLA_UNREAL_ENGINE_PATH}/GenerateProjectFiles.sh
    -project=${CARLA_UE_PROJECT_PATH}
    -CMakefile
    -game
    -engine
  )
  set (
    CARLA_UE_BUILD_COMMAND_PREFIX
      ${CARLA_UNREAL_ENGINE_PATH}/Engine/Build/BatchFiles/${UE_SYSTEM_NAME}/Build.sh
  )
  set (
    CARLA_UE_UAT_COMMAND_PREFIX
      ${CARLA_UNREAL_ENGINE_PATH}/Engine/Build/BatchFiles/RunUAT.sh
  )
endif ()



if (CMAKE_TOOLCHAIN_FILE)
  cmake_path (
    ABSOLUTE_PATH
      CMAKE_TOOLCHAIN_FILE
    BASE_DIRECTORY
      ${CARLA_WORKSPACE_PATH}
    NORMALIZE
    OUTPUT_VARIABLE
      TOOLCHAIN_FILE
  )
  set (
    TOOLCHAIN_FILE_OPTION
    --toolchain=${TOOLCHAIN_FILE}
  )
else ()
  set (TOOLCHAIN_FILE_OPTION)
endif ()



carla_add_custom_target (
  carla-ue-carla-plugin-configure
  "Configure the Carla plugin subproject."
)

carla_add_custom_target (
  carla-ue-carla-tools-plugin-configure
  "Configure the Carla plugin subproject."
)



carla_add_custom_target (
  carla-unreal
  "Build the CarlaUnreal subproject."
  COMMAND
    ${CARLA_UE_BUILD_COMMAND_PREFIX}
    CarlaUnreal
    ${UE_SYSTEM_NAME}
    ${CARLA_UNREAL_BUILD_TYPE}
    -project=${CARLA_UE_PROJECT_PATH}
    -game
    -buildscw
  DEPENDS
    ${CARLA_UNREAL_CONFIGURE_OUTPUTS}
  USES_TERMINAL
  VERBATIM
)

set_target_properties (
  carla-unreal
  PROPERTIES
    UE_SOURCE_DIR
    ${CARLA_UE_PLUGINS_PATH}
)

carla_define_ue_property (PublicDefinitions)
carla_define_ue_property (PrivateDefinitions)
carla_define_ue_property (PublicIncludePaths)
carla_define_ue_property (PrivateIncludePaths)
carla_define_ue_property (PublicDependencyModuleNames)
carla_define_ue_property (PrivateDependencyModuleNames)
carla_define_ue_property (PublicAdditionalLibraries)
carla_define_ue_property (PrivateAdditionalLibraries)
carla_define_ue_property (DynamicallyLoadedModuleNames)

carla_add_ue_PrivateIncludePaths (
  carla-unreal
  ${sqlite3_SOURCE_DIR}
)

foreach (UE_DEP ${UE_DEPENDENCIES})
  carla_add_ue_PrivateIncludePaths (
    carla-unreal
    $<TARGET_PROPERTY:${UE_DEP},INTERFACE_INCLUDE_DIRECTORIES>
  )
endforeach ()

carla_add_ue_PublicAdditionalLibraries (
  carla-unreal
  $<TARGET_FILE:libsqlite3>
)

foreach (UE_DEP ${UE_DEPENDENCIES})
  get_target_property (UE_DEP_TYPE ${UE_DEP} TYPE)
  if (NOT ("${UE_DEP_TYPE}" STREQUAL "INTERFACE_LIBRARY"))
    carla_add_ue_PublicAdditionalLibraries (carla-unreal $<TARGET_FILE:${UE_DEP}>)
  endif ()
endforeach ()

carla_add_ue_PrivateDefinitions (
  carla-unreal
  WITH_ROS2
)

carla_add_ue_PrivateDefinitions (
  carla-unreal
  WITH_OSM2ODR
)

carla_generate_ue_property_files (carla-unreal)

carla_add_custom_target (
  carla-unreal-editor
  "Build the CarlaUnrealEditor subproject."
  COMMAND
    ${CARLA_UE_BUILD_COMMAND_PREFIX}
    CarlaUnrealEditor
    ${UE_SYSTEM_NAME}
    ${CARLA_UNREAL_BUILD_TYPE}
    -project=${CARLA_UE_PROJECT_PATH}
    -game
    -buildscw
  USES_TERMINAL
  VERBATIM
)

add_dependencies (
  carla-unreal-editor
  ${UE_DEPENDENCIES_ORDER_ONLY}
)

function (
  add_carla_ue_package_target
  PACKAGE_CONFIGURATION
  UE_BUILD_CONFIGURATION)

  if (NOT "${PACKAGE_CONFIGURATION}" STREQUAL "")
    set (TARGET_NAME_SUFFIX -${PACKAGE_CONFIGURATION})
  endif ()

  carla_add_custom_target (
    carla-unreal-package${TARGET_NAME_SUFFIX}
    "Create a CARLA package in ${PACKAGE_CONFIGURATION} mode."
    COMMAND
      ${CARLA_UE_BUILD_COMMAND_PREFIX}
      CarlaUnreal
      ${UE_SYSTEM_NAME}
      ${UE_BUILD_CONFIGURATION}
      -project=${CARLA_UE_PROJECT_PATH}
      -game
      -buildscw
    COMMAND
      ${CARLA_UE_UAT_COMMAND_PREFIX}
      BuildCookRun
      -project=${CARLA_UE_PROJECT_PATH}
      -nocompileeditor
      -nop4
      -cook
      -stage
      -archive
      -package
      -iterate
      -clientconfig=${UE_BUILD_CONFIGURATION}
      -TargetPlatform=${UE_SYSTEM_NAME}
      -Platform=${UE_SYSTEM_NAME}
      -prereqs
      -build
      -archivedirectory=${CARLA_PACKAGE_PATH}
    USES_TERMINAL
    VERBATIM
  )

  set (CARLA_TARGET_PACKAGE_PATH ${CARLA_PACKAGE_PATH}/${UE_SYSTEM_NAME})

  add_custom_command (
    TARGET
      carla-unreal-package${TARGET_NAME_SUFFIX}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "********** PACKAGE CREATING VERSION FILE UNREAL PACKAGE EXTRA FILES STARTED **********"
    COMMAND ${CMAKE_COMMAND}
      -DCARLA_SOURCE_DIR=${CARLA_SOURCE_DIR}
      -DCARLA_UNREAL_ENGINE_PATH=${CARLA_UNREAL_ENGINE_PATH}
      -DCARLA_PACKAGE_VERSION_FILE=${CARLA_TARGET_PACKAGE_PATH}/VERSION
      -P${CMAKE_CURRENT_SOURCE_DIR}/Package/CreateCarlaVersionFile.cmake
    COMMAND ${CMAKE_COMMAND} -E echo "********** PACKAGE VERSION FILE UNREAL PACKAGE EXTRA FILES COMPLETED **********"

    COMMAND ${CMAKE_COMMAND} -E echo "********** PACKAGE REMOVING UNREAL PACKAGE EXTRA FILES STARTED **********"
    COMMAND ${CMAKE_COMMAND}
      -DCARLA_TARGET_PACKAGE_PATH=${CARLA_TARGET_PACKAGE_PATH}
      -P${CMAKE_CURRENT_SOURCE_DIR}/Package/RemoveUnrealPackageExtraFiles.cmake
    COMMAND ${CMAKE_COMMAND} -E echo "********** PACKAGE REMOVING UNREAL PACKAGE EXTRA FILES COMPLETED **********"

    COMMAND ${CMAKE_COMMAND} -E echo "********** PACKAGE COPYING CARLA ADDITIONAL FILES STARTED **********"
    COMMAND ${CMAKE_COMMAND}
      -DCARLA_SOURCE_DIR=${CARLA_SOURCE_DIR}
      -DCARLA_TARGET_PACKAGE_PATH=${CARLA_TARGET_PACKAGE_PATH}
      -DCARLA_BINARY_DIR=${CARLA_BINARY_DIR}
      -P${CMAKE_CURRENT_SOURCE_DIR}/Package/CopyCarlaAdditionalFiles.cmake
    COMMAND ${CMAKE_COMMAND} -E echo "********** PACKAGE COPYING CARLA ADDITIONAL FILES COMPLETED **********"

    COMMAND ${CMAKE_COMMAND} -E echo "********** COMPRESSING PACKAGE STARTED **********"
    COMMAND ${CMAKE_COMMAND}
      -DCARLA_PACKAGE_PATH=${CARLA_PACKAGE_PATH}
      -DCARLA_TARGET_PACKAGE_PATH=${CARLA_TARGET_PACKAGE_PATH}
      -DTARGET_NAME_SUFFIX=${TARGET_NAME_SUFFIX}
      -P${CMAKE_CURRENT_SOURCE_DIR}/Package/Compress.cmake
    COMMAND ${CMAKE_COMMAND} -E echo "********** COMPRESSING PACKAGE COMPLETED **********"
  )

  add_dependencies (
    carla-unreal-package${TARGET_NAME_SUFFIX}
    ${UE_DEPENDENCIES_ORDER_ONLY}
  )

  carla_add_custom_target (
    package${TARGET_NAME_SUFFIX}
    "Create a CARLA package in ${PACKAGE_CONFIGURATION} mode."
  )

  add_dependencies (
    package${TARGET_NAME_SUFFIX}
    carla-unreal-package${TARGET_NAME_SUFFIX}
  )

endfunction()

# Docs for UE5 build configurations:
# https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/DevelopmentSetup/BuildConfigurations/
add_carla_ue_package_target("" Shipping)
add_carla_ue_package_target(Shipping Shipping)
add_carla_ue_package_target(Debug Debug)
add_carla_ue_package_target(DebugGame DebugGame)
add_carla_ue_package_target(Development Development)
add_carla_ue_package_target(Test Test)



set (
  CARLA_LAUNCH_TARGET_OPTIONS
  COMMENT
    "Launching Carla Unreal..."
  COMMAND
    ${UNREAL_EDITOR_PATH}
      ${CARLA_UE_PROJECT_PATH}
      -${CARLA_UNREAL_RHI}
      ${LAUNCH_ARGS}
  USES_TERMINAL
  VERBATIM
)

carla_add_custom_target (
  launch
  "Build and open CARLA in the Unreal Editor."
  ${CARLA_LAUNCH_TARGET_OPTIONS}
)

if (WIN32)
  set (
    PRELAUNCH_CONTENT_CHECK_COMMAND
    if exist ${CARLA_UE_CONTENT_CHECK_PATH} (exit 0) else (exit 1)
  )
else ()
  set (
    PRELAUNCH_CONTENT_CHECK_COMMAND
    test -d ${CARLA_UE_CONTENT_CHECK_PATH}
  )
endif ()

carla_add_custom_target (
  check-unreal-content
  "Perform some basic checks to ensure that the CARLA Unreal Editor will not open without its assets."
  COMMAND
    ${PRELAUNCH_CONTENT_CHECK_COMMAND}
  COMMENT
    "Performing prelaunch check for CARLA Content presence."
  USES_TERMINAL
  VERBATIM
)

add_dependencies (
  launch
  carla-unreal-editor
  check-unreal-content
)

carla_add_custom_target (
  launch-only
  "Open CARLA in the Unreal Editor. This will not rebuild outdated targets."
  ${CARLA_LAUNCH_TARGET_OPTIONS}
)

add_dependencies (
  launch-only
  check-unreal-content
)

set_property (
  DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
  "${CARLA_UE_PATH}/Binaries"
  "${CARLA_UE_PATH}/Build"
  "${CARLA_UE_PATH}/DerivedDataCache"
  "${CARLA_UE_PATH}/Intermediate"
  "${CARLA_UE_PATH}/Saved"
  "${CARLA_UE_CARLA_TOOLS_PATH}/Binaries"
  "${CARLA_UE_CARLA_TOOLS_PATH}/Intermediate"
  "${CARLA_UE_CARLA_TOOLS_PATH}/Saved"
  "${CARLA_UE_CARLA_EXPORTER_PATH}/Binaries"
  "${CARLA_UE_CARLA_EXPORTER_PATH}/Intermediate"
  "${CARLA_UE_CARLA_EXPORTER_PATH}/Saved"
  "${CARLA_UE_CARLA_PATH}/Binaries"
  "${CARLA_UE_CARLA_PATH}/Intermediate"
  "${CARLA_UE_CARLA_PATH}/Saved"
  "${CARLA_UE_PLUGINS_PATH}/StreetMap/Binaries"
  "${CARLA_UE_PLUGINS_PATH}/StreetMap/Intermediate"
  "${CARLA_UE_PLUGINS_PATH}/StreetMap/Saved"
  "${CARLA_BUILD_PATH}/Package"
  "${CARLA_BUILD_PATH}/PythonAPI/dist"
)

