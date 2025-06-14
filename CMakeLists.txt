cmake_minimum_required(VERSION 3.10)
project(swipl-nlp)

include("../cmake/PrologPackage.cmake")
add_subdirectory(libstemmer_c)

AC_CHECK_FUNCS(wcsdup)

configure_file(config.h.cmake config.h)

swipl_plugin(
    double_metaphone
    C_SOURCES double_metaphone.c
    PL_LIBS double_metaphone.pl)
target_compile_definitions(plugin_double_metaphone PRIVATE __SWI_PROLOG__)

swipl_plugin(
    porter_stem
    C_SOURCES porter_stem.c
    PL_LIBS porter_stem.pl)

swipl_plugin(
    isub
    C_SOURCES isub.c pl-isub.c
    PL_LIBS isub.pl)

swipl_plugin(
    snowball
    C_SOURCES snowball.c
    THREADED C_LIBS libstemmer
    PL_LIBS snowball.pl)

add_custom_target(nlp)
add_dependencies(nlp double_metaphone porter_stem isub snowball)

pkg_doc(nlp
	SECTION
	    snowball.pl isub.pl)

test_libs(nlp)
