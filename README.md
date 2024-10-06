# Base conv

A program to convert a number between 1 and 999,999,999 between bases from
2 to 36.

## Quick Start

1. Clone the repository:
    ```bash
    git clone https://github.com/mattiarmston/base-conv.git
    ```
2. Navigate to the instillation directory
    ```bash
    cd base-conv
    ```
3. Build the project
   ```bash
   make
   ```

## Usage

`base-conv NUMBER BASE_FROM BASE_TO`

### Examples

Convert 156 from base 10 to base 16:
```bash
$ ./base-conv 255 10 16
FF
```

Convert 10110111 from binary to decimal:
```bash
$ ./base-conv 11111111 bin dec
255
```
See `base-conv --help` for more details.

## Source code

Source code can be found at [github.com/mattiarmston/base-conv](https://github.com/mattiarmston/base-conv)
