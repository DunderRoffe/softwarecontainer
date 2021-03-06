.. _api:

API
***

This chapter documents the D-Bus API for interacting with the
SoftwareContainerAgent. This is mostly useful for integrators.

.. _dbus-api:

D-Bus API
=========

D-Bus API is an IPC interface to call SoftwareContainer agent methods. The API
provides following object path and interface.

:Object Path: /com/pelagicore/SoftwareContainer
:Interface: com.pelagicore.SoftwareContainerAgent

Error Handling
--------------
All methods in this API can and will return D-Bus errors on failure. All calls
that don't return a D-Bus error have worked successfully. Any call that returns
a D-Bus error has failed. All calls that change a state and raise a D-Bus error
shall have their side effects documented in this chapter.

Methods
-------

BindMount
---------
Binds a path on the host to the container. Any missing parent directories in
the path will be created.

:Parameters:
        :containerID: ``int32`` The ID obtained by CreateContainer method.
        :pathInHost: ``string`` absolute path in the host.
        :pathInContainer: ``string`` the absolute path to mount to in the container.
        :readOnly: ``bool`` indicates whether the directory is read-only or not.

|

Create
------
Creates a container with given configuration.

:Parameters:
        :config: ``string`` config file in json format.

                Example config JSON::

                [{"enableWriteBuffer": true}]

|

:Return Values:
        :containerID: ``int32`` ID of created SoftwareContainer.

Destroy
-------
Tears down all active gateways related to container and shuts down the
container with all reserved sources.

:Parameters:
        :containerID: ``int32`` The ID obtained by CreateContainer method.

|

.. _dbus-execute:

Execute
-------
Launches the specified application/code in the container.

:Parameters:
        :containerID: ``int32`` The ID obtained by CreateContainer method.
        :commandLine: ``string`` the method to run in container.
        :workDirectory: ``string`` path to working directory.
        :outputFile: ``string`` output file to direct stdout.
        :env: ``map<string, string>`` environment variables and their values.

|

:Return Values:
        :pid: ``int32`` PID of process running in the container, as seen by the host.

List
----
Returns a list of the current containers

:Return Value:
        :containers: ``array<int32>`` IDs for all containers

ListCapabilities
----------------
Lists all capabilities that the user can apply. Note that this does not include
the default capabilities, which are not listable.

:Return Value:
        :capabilities: ``array<string>`` all available capability names

Resume
------
Resumes a suspended container

:Parameters:
        :containerID: ``int32`` The ID obtained by CreateContainer method.

|

SetCapabilities
---------------
Applies the given list of capability names to the container. Capabilities are
mapped to gateway configurations and applied to each gateway for which they
map a configuration.

:Parameters:
        :containerID: ``int32`` The ID obtained by CreateContainer method.
        :capabilities: ``array<string>`` of capability names

|

Suspend
-------
Suspends all execution inside a given container.

:Parameters:
        :containerID: ``int32`` The ID obtained by CreateContainer method.

|

Signals
-------

ProcessStateChanged
-------------------
The D-Bus API sends signal when process state is changed. There are four values to be emitted.

:Parameters:
        :containerID: ``int32`` The ID obtained by CreateContainer method.
        :processID: ``uint32`` Pocess ID of container.
        :isRunning: ``bool`` Whether the process is running or not.
        :exitCode: ``uint32`` exit code of Process.

|


Introspection
-------------

Using ``org.freedesktop.DBus.Introspectable.Introspect`` interface, methods of
SoftwareContainerAgent D-Bus API can be observed.
