# _Farmafit_
_Farmafit_ models and illustrates releasing of pharmacological substances.

Screen shot of the GUI version:

![Screenshot](/docs/sshot-gui.png "Screenshot GUI")

Screen shot of the command-line version:

![Screenshot](/docs/sshot-cli.png "Screenshot Terminal")

## Video Usage Guide

For a brief guide on how to use Farmati, see
[Farmafit Video Guide on YouTube](https://www.youtube.com/watch?v=CpnMuYuMBLU "Farmafit Video Guide")

## Installation on Windows

Windows installers are `releases/farmafit-X.Y-installer.exe` where X.Y signifies version number. `*.zip` files in `releases` folder contain all files required to run the application without installation.

## Compiling from Source

To compile, execute `make` in the root directory. See `Makefile` for further details.

All development and testing was done on MSYS2. No special configuration provisions are needed.

## Libraries Used

_cJSON_, the ultraweight JSON parser, [github.com/DaveGamble/cJSON](https://github.com/DaveGamble/cJSON), is used to process the input JSON file.

_Slope_, the C/GTK+ data visualization library, [github.com/bytebrew/slope](https://github.com/bytebrew/slope), is used for plotting in the GUI version.

## Command-line Version Usage

Create an input file which contains measured data points, in JSON format and using a plain text editor. Use `example.json` in the root directory as a model. You can give this input file any name that you like.

In the input JSON file, for _"experiment_name"_, enter a suitable experiment name/identifier (enclosed in double quotation marks). This identifier is used only for clearer reporting of results.

For each of _"data_points"_, enter _"minutes"_ and _"percentage"_. Minutes is the number of minutes elapsed from the start of the experiment (when _"minutes"_ = 0), and _"percentage"_ is the percentage of the substance dissolved at that time. Values for _"minutes"_ and _"percentage"_ are entered without double quotes.

Program assumes the measurement values are sorted in ascending order of _"minutes"_ values.

While the code will produce some results with a single data point, those results will not make much sense. Note that, for some models, we cannot use data points where _"mins"_ = 0 and/or _"percentage"_ = 0. This is because we apply the natural logarithm function to these values, and `log(0)` is not usefully defined. For models where the `log` function is used, _Farmafit_ simply eliminates the first data point, with the assumption that the first data point probably contains zero-valued _"minutes"_ and/or _"percentage"_.

There is no upper limit as to how many data points can be entered.

Use ASCII characters only. Decimal point separator (.) must be used for rational numbers. Decimal comma separator (,) is not supported.

Once the experimental data is entered into the input file, execute `./farmafit -f file_name`, where `file_name` is the name of the input file. Note that the `-f` switch must be used. Relative and absolute paths can be used. Once the input file is processed, _Farmafit_ displays the results on the screen.

### Example input file

Here is the content of the `example.json` file, which can be found in the _Farmafit_ root directory.

```
{ "experiment_name": "Experiment F1",
  "data_points":
  [
      { "minutes": 0,   "percentage": 0      },
      { "minutes": 15,  "percentage": 8.803  },
      { "minutes": 30,  "percentage": 11.935 },
      { "minutes": 45,  "percentage": 15.316 },
      { "minutes": 60,  "percentage": 19.195 },
      { "minutes": 90,  "percentage": 28.426 },
      { "minutes": 120, "percentage": 35.830 },
      { "minutes": 150, "percentage": 41.672 },
      { "minutes": 180, "percentage": 49.569 },
      { "minutes": 240, "percentage": 62.338 },
      { "minutes": 300, "percentage": 72.883 },
      { "minutes": 360, "percentage": 79.588 }
  ]
}
```

### Example output

The following is displayed on the screed after running `./farmafit -f example.json`:

```
Results of processing "Experiment F1"

Zero-order kinetics	k0 = 0.2220	rsq = 0.9839
First-order kinetics	k1 = 0.0061	rsq = 0.8701
Higuchi's equation	kh = 4.5271	rsq = 0.9661
Peppas' equation	k  = 1.0022	rsq = 0.9905
			n  = 0.7449
```

## Models and _Farmafit_ output

_Farmafit_ calculates parameters of the models listed in this section. In all equations, `Q` represents the percentage of the dissolved substance, and `t` is elapsed time in minutes.

For all models, r-squared `rsq` value is used to measure the goodness of fit.

### Zero-order kinetics

Given model equation: `Q = k0 * t`

_Farmafit_ calculates `k0`.

### First-order kinetics

Given model equation: `Q = (1 - exp (-k1 * t)) * 100`

_Farmafit_ calculates `k1`.

### Higuchi's equation

Given model equation: `Q = kh * sqrt (t)`

_Farmafit_ calculates `kh`.

### Peppas' equation

Given model equation: `Q = k * t^n`

_Farmafit_ calculates `k` and `n`.

## Documentation

See [doxygen/html/index.html](/doxygen/html/index.html) for source code documentation.

