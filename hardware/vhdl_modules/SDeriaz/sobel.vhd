-- EmbHardw
-- Sébastien Deriaz
-- 01.12.2021
--
-- Sobel complete custom instruction
--
-- dataa and datab contains 4 pixels each (8 bit per pixel)
-- 
-- 1) extract pixels from dataa and datab :
--      [P00 P01 P02]
--      [P10  -  P12]
--      [P20 P21 P22]
-- 2) apply masks for X and Y (see C code)
-- 3) Sum results for X and Y
-- 4) if sum is above threshold -> 1 else -> 0
-- 
-- The process takes 1 clk to complete
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity sobel is
    port (
        -- Avalon custom interface signals
        Clk    : in std_logic;
        reset  : in std_logic;
        dataa  : in std_logic_vector(31 downto 0);
        datab  : in std_logic_vector(31 downto 0);
        start  : in std_logic;
        result : out std_logic_vector(31 downto 0);
        done   : out std_logic
    );
end sobel;

architecture comp of sobel is
    constant threshold : signed(8 downto 0) := to_signed(127, 9);

    signal P00         : std_logic_vector(7 downto 0); -- x(-1)  y(1)
    signal P01         : std_logic_vector(7 downto 0); -- x(0)   y(2)
    signal P02         : std_logic_vector(7 downto 0); -- x(1)   y(1)
    signal P10         : std_logic_vector(7 downto 0); -- x(-2)  y(0)
    signal P12         : std_logic_vector(7 downto 0); -- x(2)   y(0)
    signal P20         : std_logic_vector(7 downto 0); -- x(-1)  y(-1)
    signal P21         : std_logic_vector(7 downto 0); -- x(0)   y(-2)
    signal P22         : std_logic_vector(7 downto 0); -- x(1)   y(-1)
    signal result_x    : signed(15 downto 0);
    signal result_y    : signed(15 downto 0);
    signal sum         : signed(15 downto 0);

begin
    -- Sobel

    -- dataa -> P00, P01, P02, P10
    P00                 <= dataa(31 downto 24);
    P01                 <= dataa(23 downto 16);
    P02                 <= dataa(15 downto 8);
    P10                 <= dataa(7 downto 0);
    -- datab -> P12, P20, P21, P22
    P12                 <= datab(31 downto 24);
    P20                 <= datab(23 downto 16);
    P21                 <= datab(15 downto 8);
    P22                 <= datab(7 downto 0);
    -- X mask
    result_x            <= - signed(P00) + signed(P02) - to_signed(2,8) * signed(P10) + to_signed(2,8) * signed(P12) - signed(P20) + signed(P22);
    -- Y mask
    result_y            <= signed(P00) + to_signed(2,8) * signed(P01) + signed(P02) - signed(P20) - to_signed(2,8) * signed(P21) - signed(P22);
    sum                 <= abs(result_x) + abs(result_y);

    result(31 downto 8) <= (others => '0');
    result(7 downto 0)  <= x"FF" when (sum > threshold) else
    x"00";

    -- Avalon interfacing
    process (Clk, reset)
    begin

        if reset = '1' then
        elsif rising_edge(Clk) then
            if (start = '1') then
                done <= '1';
            else
                done <= '0';
            end if;
        end if;
    end process;
end comp;