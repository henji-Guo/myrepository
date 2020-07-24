-- 192.168.4.1 or http://example.com/
print("---------------web wifi config----------------")
enduser_setup.start(
  function()
    enduser_setup.stop()
    print("Connected to WiFi as:" .. wifi.sta.getip())
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
  end,
  function(err, str)
    print("enduser_setup: Err #" .. err .. ": " .. str)
  end,
  print -- Lua print function can serve as the debug callback
)
