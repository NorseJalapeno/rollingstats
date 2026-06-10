# rollingstats

`rollingstats` is a Python package for rolling-window statistics
with a compiled C backend.

## Project goal

The goal of the project is to provide a small but realistic scientific Python
package. The package exposes a clean Python API and delegates the most important
rolling-window computations to C.

## Quick example

```python
import rollingstats as rs

x = [1, 2, 3, 4, 5]

print(rs.rolling_mean(x, 3))
```

Output:

```text
[2.0, 3.0, 4.0]
```

## API overview

### `backend_info()`

Returns a short string confirming that the compiled C backend is available.

### `rolling_sum(x, window)`

Computes sums over all full rolling windows.

### `rolling_mean(x, window)`

Computes arithmetic means over all full rolling windows.

### `rolling_min(x, window)`

Computes minima over all full rolling windows.

### `rolling_max(x, window)`

Computes maxima over all full rolling windows.

### `rolling_var(x, window)`

Computes population variances over all full rolling windows. The function uses
`ddof=0`.

### `rolling_std(x, window)`

Computes population standard deviations over all full rolling windows.

### `rolling_zscore(x, window)`

Computes the local z-score of the last element of every full rolling window.

### `rolling_outliers(x, window, threshold=3.0)`

Returns a list of boolean values. A value is `True` when the absolute local
z-score is greater than `threshold`.

## Tests

The project uses `pytest`.

```bash
python -m pytest
```

## Future work

Possible future features:

- rolling correlation,
- rolling slope,
- rolling median,
- rolling quantiles,
- grouped rolling statistics.
