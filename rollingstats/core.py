from __future__ import annotations

from collections.abc import Sequence
from numbers import Integral, Real

from . import _crolling


def backend_info() -> str:
    return _crolling.backend_info()


def _validate_sequence(x: Sequence[Real]) -> None:
    if isinstance(x, (str, bytes)):
        raise TypeError("x must be a one-dimensional sequence of numbers, not a string")

    try:
        n = len(x)
    except TypeError as exc:
        raise TypeError("x must be a one-dimensional sequence of numbers") from exc

    if n == 0:
        raise ValueError("x must not be empty")


def _validate_window(x: Sequence[Real], window: int) -> None:
    _validate_sequence(x)

    if not isinstance(window, Integral):
        raise TypeError("window must be an integer")

    if window <= 0:
        raise ValueError("window must be positive")

    if window > len(x):
        raise ValueError("window must not be larger than len(x)")


def _validate_threshold(threshold: float) -> None:
    if not isinstance(threshold, Real):
        raise TypeError("threshold must be a real number")

    if threshold <= 0:
        raise ValueError("threshold must be positive")


def rolling_sum(x: Sequence[Real], window: int) -> list[float]:
    _validate_window(x, window)
    return _crolling.rolling_sum(x, int(window))


def rolling_mean(x: Sequence[Real], window: int) -> list[float]:
    _validate_window(x, window)
    return _crolling.rolling_mean(x, int(window))


def rolling_min(x: Sequence[Real], window: int) -> list[float]:
    _validate_window(x, window)
    return _crolling.rolling_min(x, int(window))


def rolling_max(x: Sequence[Real], window: int) -> list[float]:
    _validate_window(x, window)
    return _crolling.rolling_max(x, int(window))


def rolling_var(x: Sequence[Real], window: int) -> list[float]:
    _validate_window(x, window)
    return _crolling.rolling_var(x, int(window))


def rolling_std(x: Sequence[Real], window: int) -> list[float]:
    _validate_window(x, window)
    return _crolling.rolling_std(x, int(window))


def rolling_zscore(x: Sequence[Real], window: int) -> list[float]:
    _validate_window(x, window)
    return _crolling.rolling_zscore(x, int(window))


def rolling_outliers(x: Sequence[Real], window: int, threshold: float = 3.0) -> list[bool]:
    _validate_window(x, window)
    _validate_threshold(threshold)

    zscores = rolling_zscore(x, window)
    return [abs(z) > threshold for z in zscores]
