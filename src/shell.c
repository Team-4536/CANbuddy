#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "zbus_can.h"

void cmd_pub_blue() {
  struct zbus_can_msg msg = {
    .source = CAN_SOURCE_RIO,
    .rio = {
        .redAlliance = 0
    }
  };
  int pub = zbus_chan_pub(&zbus_can, &msg, K_NO_WAIT);
}

void cmd_pub_red() {
    struct zbus_can_msg msg = {
      .source = CAN_SOURCE_RIO,
      .rio = {
          .redAlliance = 1
      }
    };
    int pub = zbus_chan_pub(&zbus_can, &msg, K_NO_WAIT);
  }

  void cmd_pub_manipulator(const struct shell *shell, size_t argc, char *argv[]) {
    if (argc < 3) {
      shell_error(shell, "missing args");
      return;
    }
    int elevator = atoi(argv[1]);
    int coral = atoi(argv[2]);
    struct zbus_can_msg msg = {
      .source = CAN_SOURCE_TEAM,
      .manipulator = {
          .elevatorPos = elevator,
          .coralState = coral
      }
    };
    int pub = zbus_chan_pub(&zbus_can, &msg, K_NO_WAIT);
  }

  SHELL_STATIC_SUBCMD_SET_CREATE(pub_subs,
    SHELL_CMD(b, NULL, "blue alliance.", cmd_pub_blue),
    SHELL_CMD(r, NULL, "red alliance.", cmd_pub_red),
    SHELL_CMD_ARG(m, NULL, "manipulator.", cmd_pub_manipulator, 3, 0),
    SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(pub, &pub_subs, "publish to zbus_can commands", NULL);
