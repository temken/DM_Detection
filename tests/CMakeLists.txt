# 0. Test executable
add_test(NAME test_Obscura_Executable COMMAND obscura config.cfg
	WORKING_DIRECTORY ${BIN_DIR})

# 1. Astronomy
add_executable(test_Astronomy test_Astronomy.cpp)
target_link_libraries(test_Astronomy 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Astronomy PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Astronomy PUBLIC -Wall -pedantic)
install(TARGETS test_Astronomy DESTINATION ${TESTS_DIR})
add_test(NAME Test_Astronomy COMMAND test_Astronomy
	WORKING_DIRECTORY ${TESTS_DIR})

# 2. Configuration
add_executable(test_Configuration test_Configuration.cpp)
target_link_libraries(test_Configuration 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Configuration PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Configuration PUBLIC -Wall -pedantic)
install(TARGETS test_Configuration DESTINATION ${TESTS_DIR})
add_test(NAME Test_Configuration COMMAND test_Configuration
	WORKING_DIRECTORY ${TESTS_DIR})

# 3. Direct_Detection
add_executable(test_Direct_Detection test_Direct_Detection.cpp)
target_link_libraries(test_Direct_Detection 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Direct_Detection PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Direct_Detection PUBLIC -Wall -pedantic)
install(TARGETS test_Direct_Detection DESTINATION ${TESTS_DIR})
add_test(NAME Test_Direct_Detection COMMAND test_Direct_Detection
	WORKING_DIRECTORY ${TESTS_DIR})

# 4. Direct_Detection_ER
add_executable(test_Direct_Detection_ER test_Direct_Detection_ER.cpp)
target_link_libraries(test_Direct_Detection_ER 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Direct_Detection_ER PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Direct_Detection_ER PUBLIC -Wall -pedantic)
install(TARGETS test_Direct_Detection_ER DESTINATION ${TESTS_DIR})
add_test(NAME Test_Direct_Detection_ER COMMAND test_Direct_Detection_ER
	WORKING_DIRECTORY ${TESTS_DIR})

# 5. Direct_Detection_Ionization
add_executable(test_Direct_Detection_Ionization test_Direct_Detection_Ionization.cpp)
target_link_libraries(test_Direct_Detection_Ionization 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Direct_Detection_Ionization PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Direct_Detection_Ionization PUBLIC -Wall -pedantic)
install(TARGETS test_Direct_Detection_Ionization DESTINATION ${TESTS_DIR})
add_test(NAME Test_Direct_Detection_Ionization COMMAND test_Direct_Detection_Ionization
	WORKING_DIRECTORY ${TESTS_DIR})

# 6. Direct_Detection_Migdal
add_executable(test_Direct_Detection_Migdal test_Direct_Detection_Migdal.cpp)
target_link_libraries(test_Direct_Detection_Migdal 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Direct_Detection_Migdal PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Direct_Detection_Migdal PUBLIC -Wall -pedantic)
install(TARGETS test_Direct_Detection_Migdal DESTINATION ${TESTS_DIR})
add_test(NAME Test_Direct_Detection_Migdal COMMAND test_Direct_Detection_Migdal
	WORKING_DIRECTORY ${TESTS_DIR})

# 7. Direct_Detection_Nucleus
add_executable(test_Direct_Detection_Nucleus test_Direct_Detection_Nucleus.cpp)
target_link_libraries(test_Direct_Detection_Nucleus 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Direct_Detection_Nucleus PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Direct_Detection_Nucleus PUBLIC -Wall -pedantic)
install(TARGETS test_Direct_Detection_Nucleus DESTINATION ${TESTS_DIR})
add_test(NAME Test_Direct_Detection_Nucleus COMMAND test_Direct_Detection_Nucleus
	WORKING_DIRECTORY ${TESTS_DIR})

# 8. Direct_Detection_Crystal
add_executable(test_Direct_Detection_Crystal test_Direct_Detection_Crystal.cpp)
target_link_libraries(test_Direct_Detection_Crystal 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Direct_Detection_Crystal PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Direct_Detection_Crystal PUBLIC -Wall -pedantic)
install(TARGETS test_Direct_Detection_Crystal DESTINATION ${TESTS_DIR})
add_test(NAME Test_Direct_Detection_Crystal COMMAND test_Direct_Detection_Crystal
	WORKING_DIRECTORY ${TESTS_DIR})

# 9. DM_Distribution
add_executable(test_DM_Distribution test_DM_Distribution.cpp)
target_link_libraries(test_DM_Distribution 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_DM_Distribution PRIVATE ${GENERATED_DIR} )
target_compile_options(test_DM_Distribution PUBLIC -Wall -pedantic)
install(TARGETS test_DM_Distribution DESTINATION ${TESTS_DIR})
add_test(NAME Test_DM_Distribution COMMAND test_DM_Distribution
	WORKING_DIRECTORY ${TESTS_DIR})

# 10. DM_Halo_Models
add_executable(test_DM_Halo_Models test_DM_Halo_Models.cpp)
target_link_libraries(test_DM_Halo_Models 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_DM_Halo_Models PRIVATE ${GENERATED_DIR} )
target_compile_options(test_DM_Halo_Models PUBLIC -Wall -pedantic)
install(TARGETS test_DM_Halo_Models DESTINATION ${TESTS_DIR})
add_test(NAME Test_DM_Halo_Models COMMAND test_DM_Halo_Models
	WORKING_DIRECTORY ${TESTS_DIR})

# 11. DM_Particle
add_executable(test_DM_Particle test_DM_Particle.cpp)
target_link_libraries(test_DM_Particle 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_DM_Particle PRIVATE ${GENERATED_DIR} )
target_compile_options(test_DM_Particle PUBLIC -Wall -pedantic)
install(TARGETS test_DM_Particle DESTINATION ${TESTS_DIR})
add_test(NAME Test_DM_Particle COMMAND test_DM_Particle
	WORKING_DIRECTORY ${TESTS_DIR})

# 12. DM_Particle_Standard
add_executable(test_DM_Particle_Standard test_DM_Particle_Standard.cpp)
target_link_libraries(test_DM_Particle_Standard 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_DM_Particle_Standard PRIVATE ${GENERATED_DIR} )
target_compile_options(test_DM_Particle_Standard PUBLIC -Wall -pedantic)
install(TARGETS test_DM_Particle_Standard DESTINATION ${TESTS_DIR})
add_test(NAME Test_DM_Particle_Standard COMMAND test_DM_Particle_Standard
	WORKING_DIRECTORY ${TESTS_DIR})

# 13. Experiments
add_executable(test_Experiments test_Experiments.cpp)
target_link_libraries(test_Experiments 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Experiments PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Experiments PUBLIC -Wall -pedantic)
install(TARGETS test_Experiments DESTINATION ${TESTS_DIR})
add_test(NAME Test_Experiments COMMAND test_Experiments
	WORKING_DIRECTORY ${TESTS_DIR})

# 14. Target_Atom
add_executable(test_Target_Atom test_Target_Atom.cpp)
target_link_libraries(test_Target_Atom 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Target_Atom PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Target_Atom PUBLIC -Wall -pedantic)
install(TARGETS test_Target_Atom DESTINATION ${TESTS_DIR})
add_test(NAME Test_Target_Atom COMMAND test_Target_Atom
	WORKING_DIRECTORY ${TESTS_DIR})

# 15. Target_Crystal
add_executable(test_Target_Crystal test_Target_Crystal.cpp)
target_link_libraries(test_Target_Crystal 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Target_Crystal PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Target_Crystal PUBLIC -Wall -pedantic)
install(TARGETS test_Target_Crystal DESTINATION ${TESTS_DIR})
add_test(NAME Test_Target_Crystal COMMAND test_Target_Crystal
	WORKING_DIRECTORY ${TESTS_DIR})

# 16. Target_Nucleus
add_executable(test_Target_Nucleus test_Target_Nucleus.cpp)
target_link_libraries(test_Target_Nucleus 
	PRIVATE
		libobscura
		gtest_main	#contains the main function
)
target_include_directories(test_Target_Nucleus PRIVATE ${GENERATED_DIR} )
target_compile_options(test_Target_Nucleus PUBLIC -Wall -pedantic)
install(TARGETS test_Target_Nucleus DESTINATION ${TESTS_DIR})
add_test(NAME Test_Target_Nucleus COMMAND test_Target_Nucleus
	WORKING_DIRECTORY ${TESTS_DIR})