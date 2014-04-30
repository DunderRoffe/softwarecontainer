#!/usr/bin/env python

"""
    Copyright (C) 2014 Pelagicore AB
    All rights reserved.
"""

import pytest

import json
import sys
import time
import re
import os

# conftest contains some fixtures we use in the tests
import conftest

from common import ComponentTestHelper


CONFIG = {
    "devices": [
        {
            "name": "/dev/random",
            "major": "1",
            "minor": "8",
            "mode": "666"
        }
    ]
}

CONFIGS = {
    "devicenode": json.dumps(CONFIG)
}

LS_APP = """
#!/bin/sh
ls -ls /dev/random > /appshared/devicenode_test_output
"""

# We keep the helper object module global so external fixtures can access it
helper = ComponentTestHelper()


class TestDeviceNodeGateway():

    def test_can_create_device_node(self, pelagicontain_binary, container_path, teardown_fixture):
        self.do_setup(pelagicontain_binary, container_path)

        containedapp_file = container_path + "/com.pelagicore.comptest/bin/containedapp"
        with open(containedapp_file, "w") as f:
            print "Overwriting containedapp..."
            f.write(LS_APP)
        os.system("chmod 755 " + containedapp_file)

        helper.pelagicontain_iface().Launch(helper.app_id())

        time.sleep(0.5)

        try:
            with open("%s/com.pelagicore.comptest/shared/devicenode_test_output" % container_path) as f:
                line = f.readline()
                print "-->", line
                regex = "\s*\d\scrw-rw-rw-\s*\d\s*root\s*root\s*\d,\s*\d.*/dev/random$"
                assert(re.match(regex, line))
        except Exception as e:
            pytest.fail("Unable to read command output, output file couldn't be opened! Exception: %s" % e)

    def do_setup(self, pelagicontain_binary, container_path):
        helper.pam_iface().test_reset_values()
        helper.pam_iface().helper_set_configs(CONFIGS)
        if not helper.start_pelagicontain(pelagicontain_binary, container_path):
            print "Failed to launch pelagicontain!"
            sys.exit(1)
        time.sleep(1)
