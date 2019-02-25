# _Farmafit_
Extracts model parameters for modelling the release of pharmacological substances.

## Configuration

All development and testing was done on Cygwin. No special configuration provisions are needed.

## Compiling

To compile, execute `make` in the root directory. See `Makefile` for further details.

## Libraries

cJSON, the ultraweight JSON parser,  [github.com/DaveGamble/cJSON](https://github.com/DaveGamble/cJSON), is used to process the input JSON file. Since this is a simple project, `cJSON.h` and `cJSON.c` are just copied to `lib`, and included via `Makefile`.

## Usage

Edit `input.json` in root directory using a plain text editor.

For _"experiment_name"_, enter a suitable experiment name/identifier (enclosed in double quotation marks). This identifier is used only for clearer reporting of results. 

For each of _"data_points"_, enter _"minutes"_ and _"percentage"_. Minutes is the number of minutes elapsed from the start of the experiment (when _"minutes"_ = 0), and _"percentage"_ is the percentage of the substance dissolved at that time. Values for _"minutes"_ and _"percentage"_ are entered without double quotes.

Program assumes the measurement values are sorted in ascending order of _"minutes"_ values.

While the code will produce some results with a single data point, those results will not make much sense. Note that, for some models, we cannot use data points where _"mins"_ = 0 and/or _"percentage"_ = 0. This is because we apply the natural logarithm function to these values, and `log(0)` is not usefully defined. For models where the `log` function is used, _Farmafit_ simply eliminates the first data point, with the assumption that the first data point probably contains zero-valued _"minutes"_ and/or _"percentage"_.

Use ASCII characters only. Decimal point separator (.) must be used for rational numbers. Decimal comma separator (,) is not supported.

Once experimental data is entered into `input.json` execute `./farmafit.exe` in the root directory. Results are displayed on the screen.

## Models and _Farmafit_ output

_Farmafit_ calculates parameters of the models listed in this section. In all equations, `Q` represents the percentage of the dissolved substance, and `t` is elapsed time in minutes.

For all models, r-squared `rsq` value is used to measure the goodness of fit.

### Zero-order kinetics

Model equation: `Q = k0 * t`

_Farmafit_ calculates `k0`.

### First-order kinetics

Model equation: `Q = (1 - exp (-k1 * t)) * 100`

_Farmafit_ calculates `k1`.

### Higuchi's equation

Model equation: `Q = kh * sqrt (t)`

_Farmafit_ calculates `kh`.

### Peppas' equation

Model equation: `Q = k * t^n`

_Farmafit_ calculates `k` and `n`.

## Documentation

See [doxygen/html/index.html](/doxygen/html/index.html) for source code documentation.

## Authors

[Associate Professor Jelena Djuris](http://www.ph.bg.ac.rs/en/about-us/associate-professors/1706/jelena-%C4%91uri%C5%A1-phd/)

[Novak Petrovic](https://github.com/novakpetrovic/)
