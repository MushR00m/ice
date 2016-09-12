#!/usr/bin/env python
# -*- coding: utf-8 -*-
# **********************************************************************
#
# Copyright (c) 2003-2016 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys

path = [ ".", "..", "../..", "../../..", "../../../..", "../../../../..", "../../../../../..",
         "../../../../../../..", "../../../../../../../..", "../../../../../../../../.." ]
head = os.path.dirname(sys.argv[0])
if len(head) > 0:
    path = [os.path.join(head, p) for p in path]
path = [os.path.abspath(p) for p in path if os.path.exists(os.path.join(p, "scripts", "TestUtil.py")) ]
if len(path) == 0:
    raise RuntimeError("can't find toplevel directory!")
sys.path.append(os.path.join(path[0], "scripts"))
import TestUtil

#
# Write config
#
if sys.version_info[0] == 2:
    configPath = "./config/\xe4\xb8\xad\xe5\x9b\xbd_client.config"
    decodedPath = configPath.decode("utf-8")
else:
    configPath = "./config/\u4e2d\u56fd_client.config"
    decodedPath = configPath # No need to decode with Python3, strings are already Unicode

TestUtil.createConfig(decodedPath, 
                      ["# Automatically generated by Ice test driver.", 
                       "Ice.Trace.Protocol=1",
                       "Ice.Trace.Network=1", 
                       "Ice.ProgramName=PropertiesClient", 
                       "Config.Path=" + configPath],
                      "utf-8")

sys.stdout.write("starting client... ")
sys.stdout.flush()
clientProc = TestUtil.startClient("test.Ice.properties.Client",startReader=False)
print("ok")
clientProc.startReader()
clientProc.waitTestSuccess()

if os.path.exists(decodedPath):
    os.remove(decodedPath)