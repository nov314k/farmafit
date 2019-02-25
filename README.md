# Farmafit
Extracts model parameters for modelling the release of pharmacological substances

## Configuration

All development and testing was done on Cygwin. No special configuration provisions are needed.

## Compiling

To compile, execute `make` in the root directory. See `Makefile` for further details.

## Libraries

cJSON, the ultraweight JSON parser,  (https://github.com/DaveGamble/cJSON)[https://github.com/DaveGamble/cJSON], is used to process the input JSON file. Since this is a simple project, `cJSON.h` and `cJSON.c` are just copied to `lib`, and included via Makefile.

## Usage

Edit `input.json` in root directory using a plain text editor.

For "experiment_name", put a suitable experiment name/identifier (enclosed in double quotation marks). This identifier is only used for clearer reporting of results. 

For each of "data_points", enter "minutes" and "percentage". Minutes is the number of minutes elapsed from the start of the experiment, and "percentage" is the percentage of the substance dissolved at that time. Values for "minutes" and "percentage" are entered without double quotes.

Program assumes the measurement values are sorted in ascending "minutes".

While the code will produce some results with a single data points, those results will not make much sense. Also, note that for some models we cannot use data points where "mins" = 0 and/or "percentage" = 0. This is because we apply the natural logarithm function to these values, and `log(0)` is not usefully defined. For models where the `log` function is used, Farmafit simply eliminates the first data point, with the assumption that zero-valued "minutes" and/or "percentage" are located there.

Use ASCII characters only. Decimal point separator (.) must be used for rational numbers. Decimal comma separator (,) is not supported.

Once experimental data is entered into `input.json` execute `./farmafit.exe` in the root directory. Results are displayed on the screen.

## Models

Farmafit calculates parameters of the following models. In all equations __Q__ represents the percentage of the dissolved substance, and __t__ is time in minutes.

For all models, r-squared `rsq` value is used to measure goodness of fit.

### Zero-order kinetics

`Q = k0 * t`

Farmafit calculates `k0`.

### First-order kinetics

`Q = (1 - exp (-k1 * t)) * 100`

Farmafit calculates `k1`.

### Higuchi's equation

`Q = kh * sqrt (t)`

Farmafit calculates `kh`.

### Peppas' equation

`Q = k * t^n`

Farmafit calculates `k` and `n`.

## Documentation

See (doxygen/html/index.html)[doxygen/html/index.html] for source code documentation.

## Authors

(Associate Professor Jelena Djuris)[http://www.ph.bg.ac.rs/en/about-us/associate-professors/1706/jelena-%C4%91uri%C5%A1-phd/]
Novak Petrovic
