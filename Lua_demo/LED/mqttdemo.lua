print("this file is mqtt.lua ========================== ")
-- m = mqtt.Client("mqtt-direct|securemode=3,signmethod=hmacsha1|", 40, "mqtt-direct&a1AiKMtUf57", "3BE07969F6995653F4FB53A86878DF0BB0928576")
m = mqtt.Client("ESP8266", 40, "GHJ", "password")
TIM2 = tmr.create()
TIM2:register(500, tmr.ALARM_AUTO,
function () 
    -- broker: a1AiKMtUf57.iot-as-mqtt.cn-shanghai.aliyuncs.com
    m:connect("47.95.249.141", 1883, false, 
        function(client)
            TIM2:stop()
            print("MQTT connected")
            client:subscribe("/LED", 0, 
                function(client) 
                    print("/LED subscribe success") 
                    client:subscribe("/RGB", 0, 
                        function(client) 
                            print("/RGB subscribe success")
                            LED_RGB_init() 
                        end)    
                end)
            kill_signal() -- kill
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



TIM3 = tmr.create()
TIM3:stop()
TIM3:register(1000, tmr.ALARM_AUTO,
function () 
pin = 1
status, temp, humi, temp_dec, humi_dec = dht.read11(pin)
if status == dht.OK then
    -- Integer firmware using this example
    --[[print(string.format("DHT Temperature:%d.%03d;Humidity:%d.%03d\r\n",
          math.floor(temp),
          temp_dec,
          math.floor(humi),
          humi_dec
    ))]]--

    -- Float firmware using this example
    print("DHT Temperature:"..temp..";".."Humidity:"..humi)
    TempeHumi = {}
    TempeHumi.Temperature = temp
    TempeHumi.Humidity = humi
    msg = sjson.encode(TempeHumi)
    m:publish("/TempHumi", msg, 0, 0) 
    

elseif status == dht.ERROR_CHECKSUM then
    print( "DHT Checksum error." )
elseif status == dht.ERROR_TIMEOUT then
    print( "DHT timed out." )
end

end
)


function message()
    m:on("message", 
    function(client,topic,message)
        if topic == "/LED"
        then
            print("the topic is : ".. topic .. " ,and this message: " .. message)     
            local msg_LED = sjson.decode(message)
            if check_table("LED_ON",msg_LED)
            then
                if msg_LED.LED_ON == true
                then
                    gpio.write(0,gpio.LOW)
                else
                    gpio.write(0,gpio.HIGH)
                end
            end
        elseif topic == "/RGB"
        then
            print("the topic is : ".. topic .. " ,and this message: " .. message)
            
            local msg_RGB = sjson.decode(message)
            local LED = RGB_8Bit_10Bit(msg_RGB)
            local a = sjson.encode(LED)
            print(a)
            LED_RGB_control(LED)
        else                
        end
    end
    )
    gpio.write(0,gpio.LOW)
    TIM3:start()
    --dofile("uartdemo.lua")
end

-- table check
function check_table(value,msg)
for k,v in pairs(msg) do
      if k == value 
      then
          print(k,v)
          return true
      end
    end
    return false
end


-- D5 LED_R
-- D6 LED_G
-- D7 LED_B
-- LED_RGB_init
function LED_RGB_init()
    pwm.setup(5, 80, 1023)
    pwm.setup(6, 80, 1023)
    pwm.setup(7, 80, 1023)
    pwm.start(5)
    pwm.start(6)
    pwm.start(7)
    print("LED_RGB init ...")
end
-- LED_RGB_control
-- LED table contain R G B
function LED_RGB_control(LED)
    pwm.setduty(5,LED.r)
    pwm.setduty(6,LED.g)
    pwm.setduty(7,LED.b)
end
-- RGB_8Bit_10Bit amplify
function RGB_8Bit_10Bit(LED)
    LED.r = 1023-(LED.r/255)*1023
    LED.g = 1023-(LED.g/255)*1023
    LED.b = 1023-(LED.b/255)*1023
    return LED
end





