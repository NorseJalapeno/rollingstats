"""rollingstats: fast rolling statistics with a C backend."""

from .core import (
    backend_info,
    rolling_sum,
    rolling_mean,
    rolling_min,
    rolling_max,
    rolling_var,
    rolling_std,
    rolling_zscore,
    rolling_outliers,
)

__version__ = "0.1.0"

__all__ = [
    "__version__",
    "backend_info",
    "rolling_sum",
    "rolling_mean",
    "rolling_min",
    "rolling_max",
    "rolling_var",
    "rolling_std",
    "rolling_zscore",
    "rolling_outliers",
]
