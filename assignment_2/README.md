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
set MIX_ENV=prod
mix phx.server
```
