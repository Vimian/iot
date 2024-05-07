# Install livebook

```
install Erlang 15.2+

git clone https://github.com/livebook-dev/livebook.git

cd livebook
mix deps.get
```

# Run test

## Open livebook

```
cd ../../../Users/caspe/Desktop/Github/livebook-dev/livebook
iex --sname test

//New terminal

cd ../../../Users/caspe/Desktop/Github/livebook-dev/livebook
set MIX_ENV=prod
mix phx.server
```

## Open script

Locate file in local storage.

# Build and flash ESP32

Open ESP-IDF 5.2 CMD

```
cd ../../../Users/caspe/Desktop/Github/Vimian/iot/assignment_2/src

idf.py build
idf.py -p COM3 flash
```
