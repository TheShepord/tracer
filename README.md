## Usage

Download Intel Pin [here](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-binary-instrumentation-tool-downloads.html).

```
tar xf pin-<your_pin_version>.tar.gz
PIN_ROOT=/path/to/pin/folder
```

Clone the tracer repository.

```
TRACER_ROOT=/path/to/tracer/folder
cd $TRACER_ROOT
make PIN_ROOT=$PIN_ROOT obj-intel64/tracer.so
cd $PIN_ROOT
./pin -t $TRACER_ROOT/obj-intel64/tracer.so -o <output_filename> -- <command_you_want_to_trace>
```
