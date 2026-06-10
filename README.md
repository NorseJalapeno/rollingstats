# rollingstats

`rollingstats` is a lightweight Python package for fast rolling-window statistics.
The API is written in Python, while the computational backend is implemented
as a compiled C extension using the CPython C API.

## Features

The package currently supports:

- `rolling_sum`
- `rolling_mean`
- `rolling_min`
- `rolling_max`
- `rolling_var`
- `rolling_std`
- `rolling_zscore`
- `rolling_outliers`

All functions use the `valid` rolling-window convention: only full windows are
returned.

For example, for `x = [1, 2, 3, 4, 5]` and `window = 3`, the processed windows are:

```text
[1, 2, 3]
[2, 3, 4]
[3, 4, 5]
```

The result length is therefore `len(x) - window + 1`.

## Example

```python
import rollingstats as rs

x = [1, 2, 3, 4, 5]

print(rs.backend_info())
print(rs.rolling_sum(x, window=3))
print(rs.rolling_mean(x, window=3))
print(rs.rolling_std(x, window=3))
```

Expected output:

```text
rollingstats C backend is working
[6.0, 9.0, 12.0]
[2.0, 3.0, 4.0]
[0.816496580927726, 0.816496580927726, 0.816496580927726]
```

## Tests

Run the test suite with:

```bash
python -m pytest
```

## Future work

Possible future extensions:

- rolling correlation,
- rolling slope,
- rolling median,
- rolling quantiles,
- grouped rolling statistics,

## License

MIT License.
