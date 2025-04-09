import requests
from datetime import date

API_KEY = "*********"
COUNTRY = "LT"

def is_holiday(check_date: date, country: str = COUNTRY) -> bool:
    url = "https://calendarific.com/api/v2/holidays"

    params = {
        "api_key": API_KEY,
        "country": country,
        "year": check_date.year,
        "month": check_date.month,
        "day": check_date.day,
    }

    response = requests.get(url, params=params)
    data = response.json()

    holidays = data.get("response", {}).get("holidays", [])
    if holidays:
        return True
    else:
        return False

today = date.today()
is_holiday(today)
