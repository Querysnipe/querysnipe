## Querysnipe

Querysnipe is a tool to search your logs. Use it in the following way:
```
Usage: querysnipe PATH TERM [START_TIME] [END_TIME]
```
For example,
```
$ querysnipe examples/edgar-scraper ERROR
{"level":"ERROR","time":"2024-10-24T13:56:29.961-0700","message":"Error scraping data for Year: 2024, Quarter: 3: failed to fetch dailyIndexEntry entries for https://www.sec.gov/Archives/edgar/data/1829805/0001829805-24-000006.txt: Error fetching request at url:https://www.sec.gov/Archives/edgar/data/1829805/0001829805-24-000006.txt, status code: 404"}
{"level":"ERROR","time":"2024-10-24T13:56:36.624-0700","message":"Failed to process daily index company.20231114.idx: failed to fetch dailyIndexEntry entries for https://www.sec.gov/Archives/edgar/data/1829805/0001829805-23-000006.txt: Error fetching request at url:https://www.sec.gov/Archives/edgar/data/1829805/0001829805-23-000006.txt, status code: 404"}
{"level":"ERROR","time":"2024-10-24T13:56:36.624-0700","message":"Error scraping data for Year: 2023, Quarter: 4: failed to fetch dailyIndexEntry entries for https://www.sec.gov/Archives/edgar/data/1829805/0001829805-23-000006.txt: Error fetching request at url:https://www.sec.gov/Archives/edgar/data/1829805/0001829805-23-000006.txt, status code: 404"}
...
```
If you are using a terminal, the search term will also be highlighted in the line. For example, ERROR would be displayed as $${\textsf{\color{magenta}ERROR}}$$ in the above example.

### Download Querysnipe

Release builds for Linux and Mac are available from the "Releases" page in this repository.

### Build Querysnipe

To build querysnipe, clone the repository and run the build script:
```
$ git clone https://github.com/Querysnipe/querysnipe.git
$ cd querysnipe
$ ./build.sh
```
The resulting executable will be placed in `build/querysnipe`. Copy this into your `/usr/bin` folder or add it to your `PATH` to run it from your shell.
