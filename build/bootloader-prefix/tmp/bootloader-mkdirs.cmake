# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/surendra/esp-idf/esp-idf/components/bootloader/subproject"
  "/home/surendra/eclipse-workspace/emb-ticket-dispenser/build/bootloader"
  "/home/surendra/eclipse-workspace/emb-ticket-dispenser/build/bootloader-prefix"
  "/home/surendra/eclipse-workspace/emb-ticket-dispenser/build/bootloader-prefix/tmp"
  "/home/surendra/eclipse-workspace/emb-ticket-dispenser/build/bootloader-prefix/src/bootloader-stamp"
  "/home/surendra/eclipse-workspace/emb-ticket-dispenser/build/bootloader-prefix/src"
  "/home/surendra/eclipse-workspace/emb-ticket-dispenser/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/surendra/eclipse-workspace/emb-ticket-dispenser/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/surendra/eclipse-workspace/emb-ticket-dispenser/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
