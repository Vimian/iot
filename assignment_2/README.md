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
cd <livebook github repo>
iex --sname test

//New terminal

set MIX_ENV=prod
mix phx.server
```

## Open script

Locate file in local storage.
