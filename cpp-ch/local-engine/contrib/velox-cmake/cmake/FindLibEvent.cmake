message(STATUS "[Y] --- Using LibEvent - Gluten Bundled ---")
set(LibEvent_FOUND TRUE)
set(LIBEVENT_LIB ch_contrib::event)
set(LIBEVENT_INCLUDE_DIR ${LIBEVENT_INCLUDE_DIRS})

