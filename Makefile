PROJECT = vpr-extract

IMAGE   = "$(PROJECT)-dev"
VOLUME  = "$(shell pwd):/var/$(PROJECT)"
USER    = "$(shell id -u):$(shell id -g)"

BUILD   = build

ifeq ($(CONFIG),)
CONFIG  = Debug
endif

ifeq ($(VERBOSE),)
VERBOSE = 1
endif

all: local-build

local-build: $(BUILD)
	cmake --build $(BUILD)
local-install: $(BUILD)
	cmake --build $(BUILD) --target install

docker-container:
	docker build . --file Dockerfile --tag $(IMAGE)
docker-session:
	docker run --interactive --tty --volume $(VOLUME) --user $(USER) $(IMAGE) /bin/bash
docker-build:
	docker run --volume $(VOLUME) --user $(USER) $(IMAGE) /bin/bash -c "make local-build CONFIG=$(CONFIG) VERBOSE=$(VERBOSE)"

$(BUILD): CMakeLists.txt
	cmake -B $@ -DCMAKE_BUILD_TYPE=$(CONFIG)

clean:
	rm -fr build
	rm -f $(shell find . -name "*.bin")
