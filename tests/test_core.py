import math

import pytest

import rollingstats as rs


def test_backend_info_returns_string():
    result = rs.backend_info()
    assert isinstance(result, str)


def test_backend_info_mentions_c_backend():
    result = rs.backend_info()
    assert "C backend" in result


def test_version_exists():
    assert rs.__version__ == "0.1.0"


def test_rolling_sum_basic():
    assert rs.rolling_sum([1, 2, 3, 4, 5], 3) == [6.0, 9.0, 12.0]


def test_rolling_sum_window_one():
    assert rs.rolling_sum([1, 2, 3], 1) == [1.0, 2.0, 3.0]


def test_rolling_sum_full_window():
    assert rs.rolling_sum([1, 2, 3], 3) == [6.0]


def test_rolling_mean_basic():
    assert rs.rolling_mean([1, 2, 3, 4, 5], 3) == [2.0, 3.0, 4.0]


def test_rolling_min_basic():
    assert rs.rolling_min([3, 1, 4, 2, 5], 3) == [1.0, 1.0, 2.0]


def test_rolling_max_basic():
    assert rs.rolling_max([3, 1, 4, 2, 5], 3) == [4.0, 4.0, 5.0]


def test_rolling_var_basic():
    result = rs.rolling_var([1, 2, 3, 4, 5], 3)
    expected = [2 / 3, 2 / 3, 2 / 3]
    assert result == pytest.approx(expected)


def test_rolling_std_basic():
    result = rs.rolling_std([1, 2, 3, 4, 5], 3)
    expected = [math.sqrt(2 / 3), math.sqrt(2 / 3), math.sqrt(2 / 3)]
    assert result == pytest.approx(expected)


def test_rolling_zscore_basic():
    result = rs.rolling_zscore([1, 2, 3, 4, 5], 3)
    expected = [
        (3 - 2) / math.sqrt(2 / 3),
        (4 - 3) / math.sqrt(2 / 3),
        (5 - 4) / math.sqrt(2 / 3),
    ]
    assert result == pytest.approx(expected)


def test_rolling_zscore_zero_std():
    assert rs.rolling_zscore([2, 2, 2, 2], 2) == [0.0, 0.0, 0.0]


def test_rolling_outliers_basic():
    result = rs.rolling_outliers([1, 2, 3, 100], 2, threshold=0.5)
    assert result == [True, True, True]


@pytest.mark.parametrize(
    "function_name",
    [
        "rolling_sum",
        "rolling_mean",
        "rolling_min",
        "rolling_max",
        "rolling_var",
        "rolling_std",
        "rolling_zscore",
    ],
)
def test_invalid_window_zero(function_name):
    function = getattr(rs, function_name)
    with pytest.raises(ValueError):
        function([1, 2, 3], 0)


@pytest.mark.parametrize(
    "function_name",
    [
        "rolling_sum",
        "rolling_mean",
        "rolling_min",
        "rolling_max",
        "rolling_var",
        "rolling_std",
        "rolling_zscore",
    ],
)
def test_window_too_large(function_name):
    function = getattr(rs, function_name)
    with pytest.raises(ValueError):
        function([1, 2, 3], 4)


@pytest.mark.parametrize(
    "function_name",
    [
        "rolling_sum",
        "rolling_mean",
        "rolling_min",
        "rolling_max",
        "rolling_var",
        "rolling_std",
        "rolling_zscore",
    ],
)
def test_empty_input(function_name):
    function = getattr(rs, function_name)
    with pytest.raises(ValueError):
        function([], 1)


def test_window_must_be_integer():
    with pytest.raises(TypeError):
        rs.rolling_sum([1, 2, 3], 2.5)


def test_input_must_be_numeric():
    with pytest.raises(TypeError):
        rs.rolling_sum([1, "bad", 3], 2)


def test_outliers_threshold_must_be_positive():
    with pytest.raises(ValueError):
        rs.rolling_outliers([1, 2, 3], 2, threshold=0)
