get_target_property(_qmake_executable Qt::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")

function(WinDeployApplication target)
	add_custom_command(TARGET ${target} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E
			env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
			--verbose 1
			--no-translations
			--no-svg
			--no-pdf
			--no-opengl
			--no-opengl-sw
			--no-compiler-runtime
			--no-system-d3d-compiler
			"$<TARGET_FILE:${target}>"
			COMMENT "Deploying Qt libraries using windeployqt for compilation target '${target}' ..."
			)
endfunction()