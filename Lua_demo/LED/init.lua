function signal()
    pwm.setup(4,3,512)
    pwm.start(4)
end

function kill_signal()
    pwm.close(4)
    gpio.mode(4,gpio.OUTPUT)
    gpio.write(4,gpio.HIGH)
end

signal()


wifi.setmode(wifi.STATION)
station_cfg = {}
station_cfg.ssid = " "
station_cfg.pwd = " "
station_cfg.save = true

if file.open("wifi_config.txt","r")
then
    print("------------------------------") 
    SSID = file.readline()
    PWD = file.readline()
    SSID = string.gsub(SSID,"\n","")
    PWD = string.gsub(PWD,"\n","")
    print("SSID: " .. SSID)
    print("PWD : " .. PWD)
    station_cfg.ssid = SSID
    station_cfg.pwd = PWD
    file.close()
    print("------------------------------")
end

---------------Default configuration--------------
if (station_cfg.ssid == " " or station_cfg.ssid == "")
then
   station_cfg.ssid = "heartbeats"
   station_cfg.pwd =  "24682468"
end

---------------Default configuration--------------

-----------------Start wifi connect------------------
wifi.sta.config(station_cfg)
wifi.sta.connect()

----------------------- TIM init ----------------------- 
Wifi_TIM = tmr.create()
Wifi_Count = 0
Wifi_TIM:register(1000, tmr.ALARM_AUTO, 
function()
    if wifi.sta.getip() == nil 
    then
        print("WIFI connecting...")
        if Wifi_Count == 5
        then
            -- start web wifi connecting
            Wifi_TIM:stop()
            print("start Web_WIFI connecting...")
            web_wifi()
        else
            Wifi_Count = Wifi_Count + 1
        end
            
    else
        Wifi_TIM:stop()
        print()
        print("WIFI connected")
        dofile("mqttdemo.lua")
    end
end
)
Wifi_TIM:start()


function web_wifi()
print("---------------web wifi config----------------")
enduser_setup.start(
  function()
    print(wifi.sta.getconfig(true).ssid)
    print(wifi.sta.getconfig(true).pwd)
    if file.open("wifi_config.txt","w+")
    then
        print("------------------------------") 
        file.writeline(wifi.sta.getconfig(true).ssid)
        file.writeline(wifi.sta.getconfig(true).pwd)
        file.close()
        print("------------------------------")
    end
    print("WIFI connected")
    dofile("mqttdemo.lua")
  end,
  function(err, str)
    print("enduser_setup: Err #" .. err .. ": " .. str)
  end,
  print -- Lua print function can serve as the debug callback
)
end

