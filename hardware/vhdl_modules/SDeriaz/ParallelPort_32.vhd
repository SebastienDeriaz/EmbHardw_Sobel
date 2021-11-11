-- EmbHardw
-- Sébastien Deriaz
-- 17.10.2021
-- Address :
-- == WRITE ==
-- 000 -> write dir
-- 010 -> write port
-- 011 -> set port (OR)
-- 100 -> cleae port (AND)
-- == READ ==
-- 000 -> dir
-- 001 -> pin
-- 010 -> port
library ieee;
use ieee.std_logic_1164.all;
-- use ieee.numeric_std.all;
entity ParallelPort_32 is
    port (
        -- Avalon interfaces signals
        Clk        : in std_logic;
        nReset     : in std_logic;
        Address    : in std_logic_vector (2 downto 0);
        ChipSelect : in std_logic;
        Read       : in std_logic;
        Write      : in std_logic;
        ReadData   : out std_logic_vector (31 downto 0);
        WriteData  : in std_logic_vector (31 downto 0);
        -- Parallel Port external interface 
        ParPort    : inout std_logic_vector (31 downto 0)
    );
end ParallelPort_32;

architecture comp of ParallelPort_32 is
    signal iRegDir  : std_logic_vector (31 downto 0);
    signal iRegPort : std_logic_vector (31 downto 0);
    signal iRegPin  : std_logic_vector (31 downto 0);
begin
    -- Avalon interfacing
    process (Clk, nReset)
    begin
        if nReset = '0' then
            iRegDir <= (others => '0'); -- Input by default
        elsif rising_edge(Clk) then
            ReadData <= (others => '0'); -- default value
            if ChipSelect = '1' then
                if (Write = '1') then -- Write cycle
                    case Address(2 downto 0) is
                        when "000"  => iRegDir  <= WriteData;
                        when "010"  => iRegPort <= WriteData;
                        when "011"  => iRegPort <= iRegPort or WriteData;
                        when "100"  => iRegPort <= iRegPort and not WriteData;
                        when others => null;
                    end case;
                elsif Read = '1' then -- Read cycle
                    case Address(2 downto 0) is
                        when "000"  => ReadData <= iRegDir;
                        when "001"  => ReadData <= iRegPin;
                        when "010"  => ReadData <= iRegPort;
                        when others => null;
                    end case;
                end if;
            end if;
        end if;
    end process;

    -- Parallel port
    pPort :
    process (iRegDir, iRegPort)
    begin
        for i in 0 to parPort'high loop
            if iRegDir(i) = '1' then
                ParPort(i) <= iRegPort(i);
            else
                ParPort(i) <= 'Z';
            end if;
        end loop;
    end process;
    -- Parallel Port Input value
    iRegPin <= ParPort;
end comp;