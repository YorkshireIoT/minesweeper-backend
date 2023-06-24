# Minesweeper Backend

A very basic backend for the game Minesweeper written in C.
Based on the [VS Code C/C++ Example Dev Container](https://github.com/microsoft/vscode-remote-try-cpp).

## Interaction

Interaction is done through 3 APIs: reset, pick and flag.

To allow easy integration into a GUI a 2D array is passed to reset to be initialised with values.
This 2D array contains the state of every point in the minesweeper grid, which would map onto how
each state should be displayed in a GUI.

## Demonstration

Included in the repository is a simple `main.c` file which wraps around the backend to provide
a basic command line interaction for testing the functionality.

## Building

The easiest way to get up and building this repository is with
[VS Code Dev Containers](https://code.visualstudio.com/docs/devcontainers/containers).

Click the link above to learn more but to get started you will need:

* VS Code installed
* Docker installed
* The Dev Container extension in VS Code installed.

Once you've got all this you can just click
[this link to get started](https://vscode.dev/redirect?url=vscode://ms-vscode-remote.remote-containers/cloneInVolume?url=https://github.com/YorkshireIoT/minesweeper-backend)

Once VS Code has cloned the repository into your Dev Container you can just press
Ctrl + Shift + B to build the sources or F5 to go straight into debugging it.

## Testing

To run the tests just run the following command:

```bash
$ make
```
