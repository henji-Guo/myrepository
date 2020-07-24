print("this file is mqtt.lua ========================== ")
m = mqtt.Client("clientid", 40, "user", "password")
TIM2 = tmr.create()
TIM2:register(1000, tmr.ALARM_AUTO,
function () 
    m:connect("47.95.249.141", 1883, false, 
        function(client)
            TIM2:stop()
            print("MQTT connected")
            client:subscribe("/topic", 0, function(client) print("subscribe success") end)
            message()
            --client:publish("/topic", "hello", 0, 0, function(client) print("sent") end)
        end,
        function(client, reason)
            --print("failed reason: " .. reason)
            print("MQTT connecting ...")
        end)
end
)
TIM2:start()

function message()
    m:on("message", 
    function(client,topic,message)
        if topic == "/topic"
        then
           print("the topic is : ".. topic .. " ,and this message: " .. message)
           if string.find(message, "LED_ON",1)
           then 
              print("find it")
                   -- gpio.mode(0, gpio.OUTPUT)
              gpio.write(0,gpio.LOW)
           else
              print("not find it")
              gpio.write(0,gpio.HIGH)
           end 
        end
    end
    )
    gpio.write(0,gpio.LOW)
    dofile("uartdemo.lua")
end
