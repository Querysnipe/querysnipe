typedef I64 Time;

static bool parse_time_field(I64 digits, String input, I64* offset, I64* out) {
  bool parsed = true;
  I64  value  = 0;
  for (I64 i = 0; i < digits; i++) {
    U8 c = input[*offset + i];
    if (is_digit(c)) {
      value = 10 * value + c - '0';
    } else {
      parsed = false;
      break;
    }
  }
  *offset += digits;
  *out     = value;
  return parsed;
}

static bool is_leap_year(I64 year) {
  return year % 100 == 0 ? (year % 400 == 0) : (year % 4 == 0);
}

static I64 get_days_in_month(I64 year, I64 month) {
  switch (month) {
  case  1: return 31;
  case  2: return is_leap_year(year) ? 29 : 28;
  case  3: return 31;
  case  4: return 30;
  case  5: return 31;
  case  6: return 30;
  case  7: return 31;
  case  8: return 31;
  case  9: return 30;
  case 10: return 31;
  case 11: return 30;
  case 12: return 31;
  default: return 0;
  };
}

static I64 leap_years_before(I64 year) {
  year--;
  return year / 4 - year / 100 + year / 400;
}

/* Returns current timezone offset from UTC in seconds. */
static Time get_timezone_offset() {
  time_t     time_raw = time(NULL);
  struct tm* date     = gmtime(&time_raw);
  date->tm_isdst      = -1;
  
  time_t time_gm = mktime(date);

  Time offset = 0;
  if (time_gm == -1) {
    println(WARN "Failed to compute timezone offset.");
  } else {
    offset = time_gm - time_raw;
  }
  return offset;
}

static String find_timestamp(String input) {
  I64 state = 0;
  for (I64 i = 0; i < input.size; i++) {
    U8 c = input[i];
    if (state < 4) {
      state = is_digit(c) ? (state + 1) : 0;
    } else if (state == 4) {
      state = !is_digit(c) ? (state + 1) : 0;
    } else if (4 < state && state < 7) {
      state = is_digit(c) ? (state + 1) : 0;
    } else if (state == 7) {
      state = !is_digit(c) ? (state + 1) : 0;
    } else if (7 < state && state < 10) {
      state = is_digit(c) ? (state + 1) : 0;
    } else if (state == 10) {
      return String(&input.data[i - 10], 10);
    } else {
      assert(false);
    }
  }
  return "";
}

static bool parse_time(Time time_offset, String input, Time* out) {
  bool parsed = input.size >= 10;
  I64  offset = 0;
  I64  year   = 0;
  I64  month  = 0;
  I64  day    = 0;
  
  parsed = parsed && parse_time_field(4, input, &offset, &year);
  offset++;
  parsed = parsed && parse_time_field(2, input, &offset, &month);
  offset++;
  parsed = parsed && parse_time_field(2, input, &offset, &day);

  parsed = parsed && 1970 <= year && year <= 2038;
  parsed = parsed && 1<= month && month <= 12;
  
  I64 max_days = get_days_in_month(year, month);
  parsed       = parsed && max_days > 0;
  parsed       = parsed && 1 <= day && day <= max_days;

  // println(INFO "year=", year, " month=", month, " day=", day);

  *out = 0;
  if (parsed) {
    I64 days = day - 1;
    for (I64 i = 1; i < month; i++) {
      days += get_days_in_month(year, i);
    }
    
    I64 years = year - 1970;
    I64 leaps = leap_years_before(year) - leap_years_before(1970);
    days     += 365 * years + leaps;

    // println(INFO "is_leap_year=", (I64) is_leap_year(year));
    // println(INFO "leaps=", leaps);

    *out = days * 24 * 60 * 60 + time_offset;
  }
  return parsed;
}
