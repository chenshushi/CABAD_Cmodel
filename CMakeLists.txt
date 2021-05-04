#***GENERAL********************************
cmake_minimum_required(VERSION 2.8)

project(Decoder_ED)

#***INCLUDE********************************
include_directories(src)
#***SOURCE*********************************
aux_source_directory(src  SOURCE)

#***EXECUTION******************************
add_executable(Decoder_ED main.cpp ${SOURCE})
