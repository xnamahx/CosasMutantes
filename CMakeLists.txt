cmake_minimum_required(VERSION 3.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../min-api/script/min-pretarget.cmake)

include_directories( 
	"${C74_INCLUDES}"
)

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/mi"
)

set(ELEMENTS_SRC
       mi/elements/resources.cc
       mi/elements/dsp/exciter.cc
       mi/elements/dsp/resonator.cc
       mi/elements/dsp/tube.cc
       mi/elements/dsp/multistage_envelope.cc
       mi/elements/dsp/part.cc
       mi/elements/dsp/voice.cc
       mi/elements/dsp/string.cc
       mi/elements/dsp/ominous_voice.cc
)

set(MI_COMMON_SRC
       mi/stmlib/dsp/units.cc
       mi/stmlib/dsp/atan.cc 
       mi/stmlib/utils/random.cc
)

set(MIELEMENTSLIB_SRC 
    ${MI_COMMON_SRC}
    ${ELEMENTS_SRC}
)

add_library(MIELEMENTSLib ${MIELEMENTSLIB_SRC} )

add_library( 
	${PROJECT_NAME} 
	MODULE
	${PROJECT_NAME}.cpp
)

target_link_libraries(${PROJECT_NAME} MIELEMENTSLib)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../min-api/script/min-posttarget.cmake)
