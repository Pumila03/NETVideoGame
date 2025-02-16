cmake_minimum_required(VERSION 3.3)
project(game_text C)
enable_testing()
include(CTest)

#positionne la variable
set(CMAKE_C_FLAGS "-std=c99 -g -O0 -Wall --coverage")
set(MEMORYCHECK_COMMAND "valgrind")
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1 ")
add_library(game game.c game_graph.c game_aux.c game_ext.c stack.c game_tools.c)


add_executable(game_text game_text.c)
add_executable(unit_tests unit_tests.c)
add_executable(game_random game_random.c)
add_executable(game_solve game_solve.c)


target_link_libraries(game_random game)
target_link_libraries(game_text game)
target_link_libraries(game_solve game)
target_link_libraries(unit_tests game)

add_test(test_game_shuffle_orientation ./unit_tests game_shuffle_orientation)
add_test(test_game_set_piece_orientation ./unit_tests game_set_piece_orientation)
add_test(test_game_new_empty ./unit_tests game_new_empty)
add_test(test_game_default ./unit_tests game_default)
add_test(test_game_get_piece_shape ./unit_tests game_get_piece_shape)
add_test(test_game_get_piece_orientation ./unit_tests game_get_piece_orientation)
add_test(test_game_equal ./unit_tests game_equal)
add_test(test_game_new ./unit_tests game_new)
add_test(test_game_get_ajacent_square ./unit_tests game_get_ajacent_square)
add_test(test_game_delete ./unit_tests game_delete)
add_test(test_game_set_piece_shape ./unit_tests game_set_piece_shape)
add_test(test_game_copy ./unit_tests game_copy)
add_test(test_game_reset_orientation ./unit_tests game_reset_orientation)
add_test(test_game_check_edge ./unit_tests game_check_edge)
add_test(test_game_is_connected ./unit_tests game_is_connected)
add_test(test_game_won ./unit_tests game_won)
add_test(test_game_has_half_edge ./unit_tests game_has_half_edge)
add_test(test_game_print ./unit_tests game_print)
add_test(test_game_default_solution ./unit_tests game_default_solution)
add_test(test_game_play_move ./unit_tests game_play_move)
add_test(test_game_is_well_paired ./unit_tests game_is_well_paired)
add_test(test_undo ./unit_tests undo)
add_test(test_redo ./unit_tests redo)
add_test(test_game_new_ext ./unit_tests game_new_ext)
add_test(test_game_new_ext_empty ./unit_tests game_new_ext_empty)
add_test(test_nb_rows ./unit_tests game_nb_rows)
add_test(test_nb_cols ./unit_tests game_nb_cols)
add_test(test_game_is_wrapping ./unit_tests game_is_wrapping)
add_test(test_game_load ./unit_tests game_load)
add_test(test_game_save ./unit_tests game_save)











