add_test (NAME DomainMapTests COMMAND python -m test.test_domain_map WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test (NAME WatchListTests COMMAND python -m test.test_watch_list WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_property (TEST DomainMapTests WatchListTests APPEND PROPERTY LABELS CI)
