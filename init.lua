----------------------- LED init ----------------------- 
print("led________")
gpio.mode(0, gpio.OUTPUT)
gpio.write(0,gpio.LOW)
print(gpio.read(0))
wifi.setmode(wifi.STATION)
station_cfg = {}
station_cfg.ssid = "360WiFi-3F8418"
station_cfg.pwd = "wzlwggw001001"
station_cfg.save = false
wifi.sta.config(station_cfg)
wifi.sta.connect()
time = 2*1000*1000
tmr.delay(time)
gpio.write(0,gpio.HIGH)
print("led________")
----------------------- LED init ----------------------- 

----------------------- TIM init ----------------------- 
TIM = tmr.create()
TIM:register(1000, tmr.ALARM_AUTO, 
function()
    if wifi.sta.getip() == nil 
    then
        print("WIFI connecting...")
        print("------------------------")
        print("GPIO0    " .. gpio.read(3))
        print("GPIO2    " .. gpio.read(4))
        print("GPIO15   " .. gpio.read(8))
        print("RXD  " .. gpio.read(9))
        print("TXD  " .. gpio.read(10))
        print("------------------------")
    else
        print(wifi.sta.getip())
        TIM:stop()
        print("WIFI connected")
        dofile("mqttdemo.lua")
    end
end
)
TIM:start()




