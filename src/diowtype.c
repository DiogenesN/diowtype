/* diowtype */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include "virtual-keyboard-unstable-v1-client-protocol.h"

/* Wayland code */
struct client_state {
	/* Globals */
	struct wl_seat *wl_seat;
	struct wl_display *wl_display;
	struct wl_registry *wl_registry;
	struct zwp_virtual_keyboard_v1 *zwp_virtual_keyboard_v1;
	struct zwp_virtual_keyboard_manager_v1 *zwp_virtual_keyboard_manager_v1;
	/* Objects */
	/* State */
};

static void registry_global(void *data, struct wl_registry *wl_registry, uint32_t name,
													const char *interface, uint32_t version) {
	(void)version;
	struct client_state *state = data;
	if (strcmp(interface, wl_seat_interface.name) == 0) {
		state->wl_seat = wl_registry_bind(wl_registry, name, &wl_seat_interface, 7);
	}
	else if (strcmp(interface, zwp_virtual_keyboard_manager_v1_interface.name) == 0) {
		state->zwp_virtual_keyboard_manager_v1 = wl_registry_bind(wl_registry, name,
												&zwp_virtual_keyboard_manager_v1_interface, 1);
	}
}

static void registry_global_remove(void *data, struct wl_registry *wl_registry, uint32_t name) {
	(void)data;
	(void)name;
	(void)wl_registry;
	/* This space deliberately left blank */
}

static const struct wl_registry_listener wl_registry_listener = {
	.global = registry_global,
	.global_remove = registry_global_remove,
};

int main(int argc, char *argv[]) {
	struct client_state state = { 0 };

	state.wl_display = wl_display_connect(NULL);
	state.wl_registry = wl_display_get_registry(state.wl_display);
	wl_registry_add_listener(state.wl_registry, &wl_registry_listener, &state);
	wl_display_roundtrip(state.wl_display);
	state.zwp_virtual_keyboard_v1 = zwp_virtual_keyboard_manager_v1_create_virtual_keyboard(
										state.zwp_virtual_keyboard_manager_v1, state.wl_seat);
	const char *filename = "/tmp/.dwzoomscale_keymap";
	// Check if the file exists
	if (access(filename, F_OK) != -1) {
		// File exists do nothing
	}
	else {
		fprintf(stderr, "Keymap filr does not exist, created kaymap\n");
		// File does not exist create keymap
		/////////////////////////////////// Generating keymap //////////////////////////////////
		struct xkb_context *context;
		struct xkb_keymap *keymap;
		char *keymap_string;

		// Create an XKB context
		context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
		if (!context) {
			fprintf(stderr, "Failed to create XKB context\n");
			return 1;
		}

		// Define a keymap (replace "us" with your desired layout)
		keymap = xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
		if (!keymap) {
			fprintf(stderr, "Failed to create XKB keymap\n");
			xkb_context_unref(context);
			return 1;
		}

		// Get the keymap as a string
		keymap_string = xkb_keymap_get_as_string(keymap, XKB_KEYMAP_FORMAT_TEXT_V1);
		if (!keymap_string) {
			fprintf(stderr, "Failed to get keymap string\n");
			xkb_keymap_unref(keymap);
			xkb_context_unref(context);
			return 1;
		}

		// Save the keymap string to a file
		FILE *file = fopen("/tmp/.dwzoomscale_keymap", "w");
		if (!file) {
			fprintf(stderr, "Failed to open file for writing\n");
			free(keymap_string);
			xkb_keymap_unref(keymap);
			xkb_context_unref(context);
			return 1;
		}

		fputs(keymap_string, file);
		fclose(file);

		// Clean up resources
		free(keymap_string);
		xkb_keymap_unref(keymap);
		xkb_context_unref(context);
	}
	int fd = open("/tmp/.dwzoomscale_keymap", O_RDONLY);
	off_t size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	zwp_virtual_keyboard_v1_keymap(state.zwp_virtual_keyboard_v1, XKB_KEYMAP_FORMAT_TEXT_V1,
																					fd,	size);
	// Typing the key provided by argv
	if (argc == 2) {
		int letter = atoi(argv[1]);
		zwp_virtual_keyboard_v1_key(state.zwp_virtual_keyboard_v1, 0, letter,
													WL_KEYBOARD_KEY_STATE_PRESSED);
		zwp_virtual_keyboard_v1_key(state.zwp_virtual_keyboard_v1, 0, letter,
													WL_KEYBOARD_KEY_STATE_RELEASED);
		wl_display_dispatch(state.wl_display);
	}
	else {
		fprintf(stderr, "No letter provided!\n");
	}
	close(fd);

	if (state.zwp_virtual_keyboard_v1) {
		zwp_virtual_keyboard_v1_destroy(state.zwp_virtual_keyboard_v1);
	}
	if (state.zwp_virtual_keyboard_manager_v1) {
		zwp_virtual_keyboard_manager_v1_destroy(state.zwp_virtual_keyboard_manager_v1);
	}
	if (state.wl_seat) {
		wl_seat_destroy(state.wl_seat);
	}
	if (state.wl_registry) {
		wl_registry_destroy(state.wl_registry);
	}
	if (state.wl_display) {
		wl_display_disconnect(state.wl_display);
	}
	///fprintf(stderr, "Wayland client terminated!\n");

    return 0;
}
