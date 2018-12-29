require "keybow"

-- Controls for Pico-8 by Pimoroni

-- Modified by David Glaude
-- Button mapping for JoyBonnet
-- Work both for "AZERTY" and "QWERTY" keyboard
-- Remove lights effect as there is no light on JoyBonnet

function setup() -- Set custom lights up
    keybow.auto_lights(false)
    keybow.clear_lights()
end

-- In Game Key mappings --

function handle_key_08(pressed) -- Joystic Down on Joy Bonnet
    keybow.set_key(keybow.DOWN_ARROW, pressed)
end

function handle_key_10(pressed) -- Joystic Right on Joy Bonnet
    keybow.set_key(keybow.RIGHT_ARROW, pressed)
end

function handle_key_09(pressed) -- Joystic Up on Joy Bonnet
    keybow.set_key(keybow.UP_ARROW, pressed)
end

function handle_key_11(pressed) -- Joystic Left on Joy Bonnet
    keybow.set_key(keybow.LEFT_ARROW, pressed)
end

function handle_key_04(pressed) -- Action key X is A on Joy Bonnet
    keybow.set_key("x", pressed)
end

function handle_key_01(pressed) -- Action key Z is B on Joy Bonnet
    keybow.set_key("w", pressed)
    keybow.set_key("z", pressed)
end

-- Meta Game Key mappings --

-- 1 = 11 => Escape
function handle_key_07(pressed) -- Escape
    keybow.set_key(keybow.ESC, pressed)
end

-- 2 = 2 => Quit the game
function handle_key_02(pressed) -- Quit
    if pressed then
        keybow.set_modifier(keybow.LEFT_META, keybow.KEY_DOWN)
        keybow.tap_key("a")
        keybow.tap_key("q")
        keybow.set_modifier(keybow.LEFT_META, keybow.KEY_UP)
    end
end

-- SELECT = 0 => Launch Splore
-- splore
-- Enter
function handle_key_00(pressed) -- Enter
    if pressed then
        keybow.tap_key("s")
        keybow.tap_key("p")
        keybow.tap_key("l")
        keybow.tap_key("o")
        keybow.tap_key("r")
        keybow.tap_key("e")
        keybow.set_key(keybow.ENTER, keybow.KEY_DOWN)
        keybow.set_key(keybow.ENTER, keybow.KEY_UP)
    end
end

-- START = 8 => Start the game
-- Cmd-Space
-- pico-8.app
function handle_key_05(pressed) -- Mute
    if pressed then
        keybow.set_modifier(keybow.LEFT_META, keybow.KEY_DOWN)
        keybow.set_key(keybow.SPACE, keybow.KEY_DOWN)
        keybow.sleep(500)
        keybow.set_key(keybow.SPACE, keybow.KEY_UP)
        keybow.set_modifier(keybow.LEFT_META, keybow.KEY_UP)
        keybow.text("Pico")
        keybow.sleep(500)
        keybow.tap_enter()
        keybow.sleep(500)
    end
end

-- X = 4
function handle_key_03(pressed) -- Mute
    if pressed then
        keybow.set_modifier(keybow.LEFT_CTRL, keybow.KEY_DOWN)
        keybow.tap_key("m")
        keybow.set_modifier(keybow.LEFT_CTRL, keybow.KEY_UP)
    end
end

-- Y = 9
function handle_key_06(pressed) -- Reload
    if pressed then
        keybow.set_modifier(keybow.LEFT_CTRL, keybow.KEY_DOWN)
        keybow.tap_key("r")
        keybow.set_modifier(keybow.LEFT_CTRL, keybow.KEY_UP)
    end
end
