#hnb-app

Simple command-line application which fetches currency exchange data from 
[Croatian National Bank API](https://www.hnb.hr/hnb-api) and prints it out to
stdout with user-specified column delimiter.

##Dependencies

Only `libcurl`

##Compiling

Simply run `make`

##Usage

    hnb-app [query] [delimiter]

###Simple Example

    ./build/hnb-app

is equivalent to

    ./build/hnb-app "" $'\t'

###Data on a specific date

    ./build/hnb-app "datum=2014-03-02"

###Data for a specific currency

    ./build/hnb-app "valuta=EUR"

Multiple currencies can be queried with

    ./build/hnb-app "valuta=EUR&valuta=USD"

###Data for a specific data range

    ./build/hnb-app "datum-od=2014-03-02&datum-do=2014-04-02"

Note that maximum range for the date is 1 year.
